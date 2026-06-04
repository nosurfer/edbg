
int main() {
  uint8_t input_buffer[] = {
    0x55,             /* push rbp */
    0x48, 0x89, 0xe5, /* mov rbp, rsp */
    0x89, 0x7d, 0xfc, /* mov DWORD PTR [rbp-0x4], edi */
    0x8b, 0x45, 0xfc, /* mov eax, DWORD PTR [rbp-0x4] */
    0x0f, 0xaf, 0xc0, /* imul eax, rax */
    0x5d,             /* pop ebp */
    0xc3,             /* ret */
  };
  size_t input_buffer_size = sizeof(input_buffer);
  std::ignore = disassembly(input_buffer, input_buffer_size);
  return 0;
}
