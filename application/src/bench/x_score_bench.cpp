#include "command_line_args.hpp"

#include <chrono>
#include <fstream>
#include <memory>
#include <mudock/compute/manager.hpp>
#include <mudock/compute/pipeline.hpp>
#include <mudock/format/reader.hpp>
#include <mudock/format/supported_format.hpp>
#include <mudock/log.hpp>
#include <mudock/molecule.hpp>
#include <mudock/mudock.hpp>

int main(int argc, char* argv[]) {
  const auto args = parse_command_line_arguments(argc, argv);

  mudock::info("Reading and parsing protein ", args.protein_path, " ...");
  auto protein =
      std::make_shared<mudock::dynamic_molecule>(mudock::parser<mudock::dynamic_molecule>(args.protein_path));

  mudock::info("Reading ligand(s) ", args.ligand_path, " ...");
  const auto in_format = mudock::parse_supported_format(args.ligand_path);
  auto input_queue     = std::make_shared<mudock::safe_queue<mudock::static_molecule>>();
  constexpr_switch<0, mudock::get_num_supported_format(), 1>(
      [&](const auto format_index) {
        const auto format = static_cast<mudock::supported_format>(format_index());
        auto input_text   = read_from_stream(std::ifstream(args.ligand_path));
        mudock::splitter<mudock::type_of_format<static_cast<mudock::supported_format>(format_index())>> split;
        auto ligands_description = split(std::move(input_text));
        ligands_description.emplace_back(split.flush());
        input_queue->initialize(ligands_description.size());

        mudock::info("Parsing ", ligands_description.size(), " ligand(s) ...");
        if constexpr (format == mudock::supported_format::ADTMOL2) {
#ifdef _OPENMP
  #pragma omp parallel for shared(input_queue)
#endif
          for (const auto& description: ligands_description) {
            auto ligand = std::make_unique<mudock::static_molecule>(
                mudock::parser<mudock::supported_format::ADTMOL2, mudock::static_molecule>(description));
            input_queue->enqueue(ligand);
          }
        } else {
          // Non-ADTMOL2 formats go through OpenBabel perception, which does not preserve the raw
          // SYBYL token X-Score relies on. The authoritative SYBYL types are carried only by the
          // native ADTMOL2 reader, so for correct X-Score typing convert ligands to .adtmol2 first.
          for (const auto& description: ligands_description) {
            try {
              auto ligand = std::make_unique<mudock::static_molecule>(
                  mudock::parser<format, mudock::static_molecule>(description));
              input_queue->enqueue(ligand);
            } catch (...) {}
          }
        }
      },
      in_format);

  input_queue->send_terminate_signal(); // signal that no more ligand will be enqueued

  mudock::info("Running X-Score (vdw) batch scoring ...");

  mudock::x_score_pipeline pipe{protein};
  auto output_queue = std::make_shared<mudock::safe_queue<mudock::static_molecule>>();
  output_queue->initialize(input_queue->size());

  const auto start = std::chrono::high_resolution_clock::now();
  {
    auto threadpool = mudock::threadpool();
    mudock::manager(args.device_confs, threadpool, args.knobs, input_queue, output_queue, pipe);
  } // threadpool destructor waits for workers; computation is complete here

  output_queue->send_terminate_signal();
  const auto end                              = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> elapsed = end - start;

  std::size_t processed = 0;
  for (auto ligand = output_queue->dequeue(); ligand; ligand = output_queue->dequeue()) {
    ++processed;
    const auto& name  = ligand->properties.get(mudock::property_type::NAME);
    const auto& score = ligand->properties.get(mudock::property_type::SCORE);
    mudock::info("Ligand ", name, " vdw score: ", score);
  }

  mudock::info("Processed ligands: ", processed);
  mudock::info("Elapsed time: ", elapsed.count(), " s");
  if (elapsed.count() > 0.0) {
    mudock::info("Throughput: ", static_cast<double>(processed) / elapsed.count(), " ligands/s");
  }

  return 0;
}
