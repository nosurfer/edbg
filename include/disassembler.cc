#pragma once

#include <cstdint>
#include <print>
#include <expected>
#include <system_error>

#include <capstone/capstone.h>

#define CAPSTONE_ARCH CS_ARCH_X86
#define CAPSTONE_MODE CS_MODE_64

std::expected<void, std::error_code> disassembly(void *shellcode_addr, size_t shellcode_size)
{
  csh handle = {};
  size_t count = 0;
  cs_insn *insn = nullptr;
  auto* code = reinterpret_cast<const uint8_t*>(shellcode_addr);

  if (cs_err err = cs_open(CAPSTONE_ARCH, CAPSTONE_MODE, &handle); err != CS_ERR_OK)
    return std::unexpected(
      std::error_code(
        static_cast<int>(err),
        std::generic_category()
      ) 
    );

  count = cs_disasm(
    handle, 
    code,
    shellcode_size, 
    reinterpret_cast<uint64_t>(shellcode_addr), 
    0, 
    &insn
  );

  if (count > 0) {
    size_t j;
    for (j = 0; j < count; j++) {
      std::print("0x{:016x} | ", insn[j].address);
      for (int k = 0; k < insn[j].size; k++)
        std::print("{:02x} ", insn[j].bytes[k]);
      for (int k = insn[j].size; k < 15; k++)
        std::print("   ");
      std::println(" | {} {}", insn[j].mnemonic, insn[j].op_str);
    }
    cs_free(insn, count);
  } else {
      std::println(stderr, "capstone: failed to disassemble shellcode");
      for (unsigned int i = 0; i <= shellcode_size; i += 16) {
        std::print("0x{:016x} | ", reinterpret_cast<uintptr_t>(code+i));
      for (int k = 0; k < 16; k++)
        std::print("{:02x} ", code[i+k]);
      std::println();
    }
  }
  cs_close(&handle);
  return {};
}
