cmd_/home/haruna/Desktop/Kawaii-stuff/Linux-driver-dev/GPIO-Driver/Module.symvers := sed 's/ko$$/o/' /home/haruna/Desktop/Kawaii-stuff/Linux-driver-dev/GPIO-Driver/modules.order | scripts/mod/modpost -m    -o /home/haruna/Desktop/Kawaii-stuff/Linux-driver-dev/GPIO-Driver/Module.symvers -e -i Module.symvers   -T -
