cmd_/home/timmy/EOS/repo/lab4/mydev.mod := printf '%s\n'   mydev.o | awk '!x[$$0]++ { print("/home/timmy/EOS/repo/lab4/"$$0) }' > /home/timmy/EOS/repo/lab4/mydev.mod
