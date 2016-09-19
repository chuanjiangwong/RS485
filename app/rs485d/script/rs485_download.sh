#This script is used to kill old pid, and download new app by tftpd
#This script is running on Pannos device

download_file="enno_rs485d enno_rs485c"
serverip=192.168.65.146



#This function is stop function
stop(){
#		ps | grep enno-bacnet-service | cut -c 0-5 | xargs kill -9
		somethings=`ps -ef | grep $1 | grep -v "grep"`
        if [ -n "$somethings" ];then
			pid=${somethings%%r*}
			kill -9 $pid
            echo "kill pid: $pid "
		fi
		retval=$?
		return $retval
}


#kill old pid
for i in $download_file
do
    stop $i
    stop $i
done


#download new app by tftpd
for i in $download_file
do
    echo "download file : $i ..."
    tftp -gr $i $serverip
    chmod +x $i
done



