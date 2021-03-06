
#!/bin/bash

<<<<<<< HEAD
pkg1="mediatomb"
pkg2="mysql"
pkg2Install="mysql-server"
pkg3="mplayer"
dbusr="mediatomb"
dbpass="mediatomb"

exitMain=0

STR_TO="localhost2"
STR_FROM="localhost"
file="/etc/mediatomb/config3.xml"
TFILE="config4.xml"

crtusr2="create user 'mediatomb'@'localhost' identified by 'mediatomb';"
crtusr="create user 'mediatomb'@'%' identified by 'mediatomb';"
crtpermiss="grant all on mediatomb.* to 'mediatomb'@'%' identified by 'mediatomb';"
crtpermiss2="grant all on mediatomb.* to 'mediatomb'@'localhost' identified by 'mediatomb';"
=======
dbuser="mediatomb"
dbpass="mediatomb"

DFLT_MT="~/.mediatomb/config.xml"
DFLT_SQL="/etc/mysql/my.cnf"

UI_TO='ui enabled="yes"'
UI_FROM='ui enabled="no"'
SQL_TO='<mysql enabled="yes"'
SQL_FROM='<mysql enabled="no"'
SQLite_TO='sqlite3 enabled="no"'
SQLite_FROM='sqlite3 enabled="yes"'

file='/tmp/BT1/config.xml'
TFILE='/tmp/BT1/config2.xml'
SQL_file='/tmp/BT1/mysqlConf'
SQL_fin='/tmp/BT1/my.cnf'
SQLBind_TO=$HOSTIP
SQLBind_FROM='127.0.0.1'
exitMain=0

FinFile="/tmp/BT1/config4.xml"

crtusr2="create user '$dbuser'@'localhost' identified by '$dbuser';"
crtusr="create user '$dbuser'@'%' identified by '$dbuser';"
crtpermiss="grant all on mediatomb.* to '$dbuser'@'%' identified by 'mediatomb';"
crtpermiss2="grant all on mediatomb.* to '$dbuser'@'localhost' identified by 'mediatomb';"
>>>>>>> e81afa4fed9cd6586e9b8ecfb48d8747dc16a715
lvmysql="exit"


crtdb="create database mediatomb"


if [ $USER != 'root' ];
then
echo "Beagle Tomb REQUIRES root in order to install"
exit 0
fi

choice=0

while [ $choice != 4 ]
do
clear
echo "Beagle Tomb Setup Menu"
echo "======================"
echo "1) Instructions"
echo "2) Install"
echo "3) Configure"
echo "4) quit"
read choice

case $choice in
2)
<<<<<<< HEAD
echo "***checking and installing missing dependencies ****!"
if ! which $pkg1 > /dev/null; then
echo "Please install $pkg1 or make sure it is in your path"
echo "attempting to install $pkg1 for you"
sudo apt-get install $pkg1 
failed_dep=1
else
echo "you have $pkg1 installed"
fi	

if ! which $pkg2 > /dev/null; then
if ! which $pkg4 > /dev/null; then
echo "Please install $pkg4 or make sure it is in your path"
echo "attempting to install $pkg4 for you"
sudo apt-get install $pkg4
failed_dep=1
else
echo "you have $pkg4 installed"
echo "Please install $pkg2 or make sure it is in your path"
echo "attempting to install $pkg2 for you"
sudo tasksel install lamp-server
fi
else
echo "you have $pkg2 installed"
if ! which $pkg3 > /dev/null; then
echo "Please install $pkg3 or make sure it is in your path"
echo "attempting to install $pkg3 for you"
sudo tasksel install $pk3
else
echo "you have $pkg3 installed"
fi
echo "***** Mysql Table and User install*****"
echo " the field below you must enter your root password to your mysql database "
echo " if you just installed mysql it's the password you set during installation"
echo " note:  this password is not stored "
=======
echo  "What is your Server's local IP?"
read HOSTIP
echo  "What is your preferred mediatomb Username?"
read dbuser
echo  "What is your preferred mediatomb Password?"
read dbpass
echo "***** Mysql Table and User install*****"
echo "***************************************"
echo "*"
echo "*"
echo " the field below you must enter your root password to your mysql database "
echo " if you just installed mysql it's the password you set during installation"
echo " note:  this password is not stored "
echo "*"
echo "*"
>>>>>>> e81afa4fed9cd6586e9b8ecfb48d8747dc16a715
mysql -u root -p << eof 
$crtusr2
$crtusr
$crtpermiss
$crtpermiss2
$lvmysql
eof
echo " mediatomb user+password+permissions created."
echo " creating table..... "
mysql -u mediatomb --password=$dbpass << eof
$crtdb
eof
echo " Filling database....."
mysql mediatomb -u mediatomb --password=$dbpass < /usr/share/mediatomb/mysql.sql
<<<<<<< HEAD
fi


if [[ $failed_dep == '1' ]]
then 
echo "run the installer again to make sure you install the rest of dependencies"

else
echo "  Install Complete "
fi
;;
3)
sh configure.sh
=======
echo " Install Complete. Hit enter return to menu. Next step:  configuration."
read fin1

;;
3)
# sh configure.sh
mkdir -p /tmp/BT/
mkdir -p /tmp/BT1/
chmod 777 /tmp/BT1/
chmod 777 /tmp/BT/
cp ~/.mediatomb/config.xml $TFILE

sed "s/$UI_FROM/$UI_TO/g" "$TFILE" > $file
sed "s/$SQL_FROM/$SQL_TO/g" "$file" > $TFILE
sed "s/$SQLite_FROM/$SQLite_TO/g" "$TFILE" > $file


sed '21q' $file > /tmp/BT/atestout.txt 
sed -e :a -e '$q;N;127,$D;ba' $file > /tmp/BT/dtestoutput.txt
echo "<host>"$HOSTIP"</host>" > /tmp/BT/btestout.txt
echo "<username>"$dbuser"</username>" > /tmp/BT/ctestout.txt
echo "<password>"$dbpass"</password>" >> /tmp/BT/ctestout.txt
cat /tmp/BT/* > $FinFile

cp $FinFile ~/.mediatomb/config.xml

sudo chown mediatomb:mediatomb ~/.mediatomb/config.xml

cp $DFLT_SQL $SQL_file

sed "s/$SQLBind_FROM/$SQLBind_TO/g" "$SQL_file" > $SQL_fin

cp -f $SQL_fin $DFLT_SQL

 rm -R /tmp/BT
 rm -R /tmp/BT1
echo " Configure Complete. Hit enter to return to menu."
read fin
>>>>>>> e81afa4fed9cd6586e9b8ecfb48d8747dc16a715
;;
1)

step=1
step1='16q'
step2='35q'
step3='70q'
step4='85q'
file='instructions'
Instchoice=0
while [ $Instchoice != "x" ]
do
	case $step in
	1)
	sed $step1 $file 
	read Instchoice 	
	step="2"
	;;
	2)
	sed $step2 $file 
	read Instchoice 
	step="3" 
	;;
	3)
	sed $step3 $file 
	read Instchoice 
	step="4"
	;;
	4)
	sed $step4 $file 
	read Instchoice 
	;;
	esac	
done
;;
esac
done
exit
