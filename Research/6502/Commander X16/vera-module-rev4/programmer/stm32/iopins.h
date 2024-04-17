#ifndef IO_DEFINE
#    define IO_DEFINE(name, bank, io, invert, mode, initial_value, is_open_drain, speed, pull_up_down, function)
#endif

// clang-format off
IO_DEFINE(IO_USB_DM,     IOBANK_A, 11, 0, IOMODE_ALT,  0, 0, IOSPEED_FAST, IOPULL_NONE, 2)
IO_DEFINE(IO_USB_DP,     IOBANK_A, 12, 0, IOMODE_ALT,  0, 0, IOSPEED_FAST, IOPULL_NONE, 2)

IO_DEFINE(IO_FPGA_CDONE, IOBANK_A,  0, 0, IOMODE_IN,   0, 0, IOSPEED_FAST, IOPULL_UP,   0)
IO_DEFINE(IO_FPGA_RESET, IOBANK_A,  1, 1, IOMODE_OUT,  1, 0, IOSPEED_FAST, IOPULL_NONE, 0)
IO_DEFINE(IO_DBG_TXD,    IOBANK_A,  2, 0, IOMODE_ALT,  0, 0, IOSPEED_FAST, IOPULL_NONE, 1) // USART2_TX
IO_DEFINE(IO_DBG_RXD,    IOBANK_A,  3, 0, IOMODE_ALT,  0, 0, IOSPEED_FAST, IOPULL_NONE, 1) // USART2_RX
IO_DEFINE(IO_FPGA_SSEL,  IOBANK_A,  4, 1, IOMODE_OUT,  0, 0, IOSPEED_FAST, IOPULL_NONE, 0)
IO_DEFINE(IO_SPI_SCK,    IOBANK_A,  5, 0, IOMODE_ALT,  0, 0, IOSPEED_FAST, IOPULL_NONE, 0) // SPI1_SCK
IO_DEFINE(IO_SPI_MISO,   IOBANK_A,  6, 0, IOMODE_ALT,  0, 0, IOSPEED_FAST, IOPULL_NONE, 0) // SPI1_MISO
IO_DEFINE(IO_SPI_MOSI,   IOBANK_A,  7, 0, IOMODE_ALT,  0, 0, IOSPEED_FAST, IOPULL_NONE, 0) // SPI1_MOSI
