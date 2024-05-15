cmd_/home/timmy/led/led_driver.mod := printf '%s\n'   led_driver.o | awk '!x[$$0]++ { print("/home/timmy/led/"$$0) }' > /home/timmy/led/led_driver.mod
