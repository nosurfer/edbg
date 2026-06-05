#include "disassembler.hpp"

std::expected<void, std::error_code> disassembly(std::span<const std::uint8_t> code, std::uintptr_t base_addr)
{
  csh handle = {};
  size_t count = 0;
  cs_insn *insn = nullptr;

  if (cs_err err = cs_open(CAPSTONE_ARCH, CAPSTONE_MODE, &handle); err != CS_ERR_OK)
    return std::unexpected(
      std::error_code(
        static_cast<int>(err),
        std::generic_category()
      ) 
    );

  count = cs_disasm(
    handle, 
    code.data(),
    code.size(), 
    base_addr,
    0, 
    &insn
  );

  if (count > 0) {
    size_t j;
    constexpr std::size_t max_insn_size = 15;
    for (j = 0; j < count; j++) {
      std::print("0x{:016x} | ", insn[j].address);
      for (std::size_t k = 0; k < insn[j].size; k++)
        std::print("{:02x} ", insn[j].bytes[k]);
      for (std::size_t k = insn[j].size; k < max_insn_size; k++)
        std::print("   ");
      std::println(" | {} {}", insn[j].mnemonic, insn[j].op_str);
    }
    cs_free(insn, count);
  } else {
    std::println(stderr, "capstone: failed to disassemble");

    for (std::size_t i = 0; i < code.size(); i += 16) {
      std::print("0x{:016x} | ", base_addr + i);

      auto line_size =
          std::min<std::size_t>(16, code.size() - i);

      for (std::size_t k = 0; k < line_size; ++k)
          std::print("{:02x} ", code[i + k]);

      std::println();
    }
  }
  cs_close(&handle);
  return {};
}
