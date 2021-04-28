Getting started:

1) Edit the load_EDIT_ME.sh Shell script
   Change the interface names to your desired interfaces
2) ./load_EDIT_ME.sh


OR manualy


1) make clean; make
2) insmod prp_dev.ko dev_name_A="ethx" dev_name_B="ethy"        
3) ifconfig prp0 <your ip>


