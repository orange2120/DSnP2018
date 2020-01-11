#!/bin/bash
RUN_DIFF=$2

if ! [ -d "td/" ]; then
	echo "td folder do not exists, create td"
	mkdir td/
fi

if ! [ -f "dofiles/$1" ]; then
	echo "[ERROR] dofile do not exists!"
	exit 0
fi

echo "===== Running mine ====="
./fraig -f dofiles/$1 >& td/$1_t.log

if [ -f "simRes.log" ]; then
	echo "Rename mine simulation file..."
	mv simRes.log td/$1_t_simRes.log
fi

echo "mine done."

echo "===== Running ref ====="
./ref/fraig -f dofiles/$1 >& td/$1_r.log

if [ -f "simRes.log" ]; then
	echo "Rename ref simulation file..."
	mv simRes.log td/$1_r_simRes.log
fi

echo "ref done."

if [ $RUN_DIFF ]; then
	echo "===== Running diff ====="
	diff td/$1_t.log td/$1_r.log
	echo "diff finished"
fi
