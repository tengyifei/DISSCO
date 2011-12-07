location=`cd $1;pwd`
if [ -e "$location/T" ]
then
echo "Path $location is a DISSCO project."
else
echo ""
echo "Error: Project path is not valid."
echo "Could not find: $location/T"
echo ""
echo "Specify relative or absolute path to the DISSCO project:"
echo "./go ../myproject"
echo ""
exit
fi

cmoddir=`pwd`

rm -f $location/*multi*
rm -f $location/SoundFiles/*multi*

echo "cd $location;$cmoddir/cmod ./ 0 18 &; $cmoddir/cmod ./ 1 18 &; $cmoddir/cmod ./ 2 18 &; $cmoddir/cmod ./ 3 18 &; $cmoddir/cmod ./ 4 18 &; $cmoddir/cmod ./ 5 18 &; hostname; exit" | ssh -T luigi

echo "cd $location;$cmoddir/cmod ./ 6 18 &; $cmoddir/cmod ./ 7 18 &; $cmoddir/cmod ./ 8 18 &; $cmoddir/cmod ./ 9 18 &; $cmoddir/cmod ./ 10 18 &; $cmoddir/cmod ./ 11 18 &; hostname; exit" | ssh -T ben

echo "cd $location;$cmoddir/cmod ./ 12 18 &; hostname; exit" | ssh -T herbert

echo "cd $location;$cmoddir/cmod ./ 13 18 &; hostname; exit" | ssh -T john

echo "cd $location;$cmoddir/cmod ./ 14 18 &; hostname; exit" | ssh -T james

echo "cd $location;$cmoddir/cmod ./ 15 18 &; hostname; exit" | ssh -T sal

echo "cd $location;$cmoddir/cmod ./ 16 18 &; hostname; exit" | ssh -T pierre

echo "cd $location;$cmoddir/cmod ./ 17 18 &; hostname; exit" | ssh -T edgard

