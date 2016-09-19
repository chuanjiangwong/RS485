#!/bin/sh -e

#This shell script is used to start or stop RS485 daemonize server.
#Date: 2016-03-07
#Auth: Chuanjiang.wong


#retruen value
retval=0

#The bacnet server PATH
enno_rs485d_service=/home/user/bin/rs485d/enno_rs485d

#The bacnet configure PATH
enno_rs485d_config=/home/user/bin/rs485d/rs485d.config



#This function is create a random, used for MSTP MAC
#random(){
#	min=$1;
#	max=`expr $2 - $1`;
#	num=$(date +%N);
#	retnum=`expr ${num} % ${max}`;
#	retnum=`expr $retnum + ${min}`;
#	((retnum=num%max+min));
#	echo $retnum;
#}


#This function is start function
start(){
		$enno_rs485d_service
		retval=$?
		return $retval
}


#This function is stop function
stop(){
#		ps | grep enno-bacnet-service | cut -c 0-5 | xargs kill -9
		rs485d=`ps -ef | grep $enno_rs485d_service | grep -v "grep"`
        if [ -n "$rs485d" ];then
			pid=${rs485d%%r*}
			kill -9 $pid
            echo "stop pid: $pid "
		fi
		retval=$?
		return $retval
}


#which argument used.
case "$1" in
	start)
		echo "#########start the RS485 service###############"
		echo "loading the configure..."

		echo "start the RS485 server..."
		start
		;;
	stop)
		echo "#########stop the RS485 servier###############"
		stop
#just for have kill it, so I maybe repeat many times
        stop
        stop
        stop
        stop
        stop
		;;
	restart)
		sh $0 stop
		sh $0 start
		;;
	*)
		echo "Format error!"
		echo $"Usage: $0 {start | stop | restart}"
		exit 1
		;;
esac

exit $retval

