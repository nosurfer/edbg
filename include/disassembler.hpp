#pragma once

#include <span>
#include <print>
#include <cstdint>
#include <expected>
#include <system_error>

#include <capstone/capstone.h>

#define CAPSTONE_ARCH CS_ARCH_X86
#define CAPSTONE_MODE CS_MODE_64

std::expected<void, std::error_code> disassembly(std::span<const std::uint8_t> code, std::uintptr_t base_addr);
