cmd_/home/timmy/led/Module.symvers :=  sed 's/ko$$/o/'  /home/timmy/led/modules.order | scripts/mod/modpost -m -a    -o /home/timmy/led/Module.symvers -e -i Module.symvers -T - 
