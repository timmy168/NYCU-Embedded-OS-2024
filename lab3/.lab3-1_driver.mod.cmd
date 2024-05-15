cmd_/home/timmy/EOS/repo/lab3/lab3-1_driver.mod := printf '%s\n'   lab3-1_driver.o | awk '!x[$$0]++ { print("/home/timmy/EOS/repo/lab3/"$$0) }' > /home/timmy/EOS/repo/lab3/lab3-1_driver.mod
