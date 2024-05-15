cmd_/home/timmy/EOS/repo/hw1/hw1_driver.mod := printf '%s\n'   hw1_driver.o | awk '!x[$$0]++ { print("/home/timmy/EOS/repo/hw1/"$$0) }' > /home/timmy/EOS/repo/hw1/hw1_driver.mod
