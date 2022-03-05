# w6502-test
Tests for w65c02 based on recordings made with a real w65c02 connected
to an Arduino controlled by a Python scrips running on the host computer.

The pyton script generates a series of small programs testing all opcodes. The re are also a set of tests that tests IRQ/NMI timing.

The bus state when the clock signal is high is recorded and stored into a number of .h files found under test/real_w65c02_recordings.

# To run tests
```
$ fips build
$ fips run w65c02-tests
```
