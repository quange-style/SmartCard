CheckProcess()
{
if [ "$1" = "" ];
then
return 1
fi

PROCESS_CNT=`ps | grep "$1" | grep -v "grep" | wc -l`

if [ $PROCESS_CNT -ge 1 ];
then
return 1
else
return 0
fi
}

cd /app
mkdir -p /app/ParamFolder /app/Log /app/Trade /app/Tmp /app/backupapp 
try_exec=0
while [ 1 ];
do
CheckProcess csafc
Check_RET=$?
if [ $Check_RET -eq 0 ];
then
 
  if [ -e /app/ParamFolder/CSReader ];then
     if [ -e /app/csafc ];then
     cp -rf /app/csafc /app/backupapp
     rm -rf /app/csafc
     fi
     mv /app/ParamFolder/CSReader /app/csafc
     sleep 2
     chmod a+x /app/csafc
  fi
  if [ -e /app/csafc ];then
    /app/csafc
  fi
  try_exec=$((try_exec+1))
  if [ $try_exec -ge 10 ];then
    cp -rf /app/backupapp/csafc /app
  fi
else
  try_exec=0
fi
sleep 1
done

