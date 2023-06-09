cmd_/home/plm/桌面/oslabfinal/v7/os.mod := printf '%s\n'   file_read.o file_struct.o final.o | awk '!x[$$0]++ { print("/home/plm/桌面/oslabfinal/v7/"$$0) }' > /home/plm/桌面/oslabfinal/v7/os.mod
