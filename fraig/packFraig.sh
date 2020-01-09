if [ -d "$1_fraig" ]; then
	echo "override..."
	rm -r $1_fraig/*
	mkdir $1_fraig/
	mkdir $1_fraig/include
	mkdir $1_fraig/src
	mkdir $1_fraig/src/util
	mkdir $1_fraig/src/cir
else
	echo "create $1_fraig..."
	mkdir $1_fraig/
	mkdir $1_fraig/include
	mkdir $1_fraig/src
	mkdir $1_fraig/src/util
	mkdir $1_fraig/src/cir
fi

cp -v Makefile $1_fraig/
cp -v src/util/myHashMap.h $1_fraig/src/util
cp -v src/util/myHashSet.h $1_fraig/src/util
cp -v src/cir/make.cir $1_fraig/src/cir/
cp -v src/cir/cirCmd.cpp $1_fraig/src/cir/
cp -v src/cir/cirFraig.cpp $1_fraig/src/cir/
cp -v src/cir/cirMgr.cpp $1_fraig/src/cir/
cp -v src/cir/cirSim.cpp $1_fraig/src/cir/
cp -v src/cir/cirCmd.h $1_fraig/src/cir/
cp -v src/cir/cirGate.cpp $1_fraig/src/cir/
cp -v src/cir/cirMgr.h $1_fraig/src/cir/
cp -v src/cir/cirDef.h $1_fraig/src/cir/
cp -v src/cir/cirGate.h $1_fraig/src/cir/
cp -v src/cir/cirOpt.cpp $1_fraig/src/cir/
echo ""
tar zcvf $1_fraig.tgz $1_fraig/
echo "\n"
./SelfCheck $1_fraig.tgz
echo ""
echo "finished!"
