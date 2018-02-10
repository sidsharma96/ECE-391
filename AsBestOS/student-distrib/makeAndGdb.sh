#!/bin/bash
sudo make && gdb bootimg -ex "target remote 10.0.2.2:1234" -ex "break tests.c:532"
