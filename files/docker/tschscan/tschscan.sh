#!/bin/sh
if [ -f channels.yml ]; then rm channels.yml; fi
if [ -f tmp.ts ]; then rm tmp.ts; fi
for ch in `seq 13 62`; do
	echo "********** ${ch} **********"
	recisdb tune --device /dev/px4video3 -t 4 -c T${ch} ./tmp.ts
	if [ -f tmp.ts ]; then
		./tschput tmp.ts | nkf --ic=CP932 | head -n 1 > tmp.txt
		cat tmp.txt
		rm tmp.ts
		while read scramble onid tsid sid channel; do
			echo "- name: $channel" >> channels.yml
			echo "  type: GR" >> channels.yml
			echo "  channel: '${ch}'" >> channels.yml
		done < tmp.txt
		rm tmp.txt
	fi
done
for ch in `seq 13 63`; do
	echo "********** C${ch} **********"
	recisdb tune --device /dev/px4video2 -t 4 -c C${ch} ./tmp.ts
	if [ -f tmp.ts ]; then
		./tschput tmp.ts | nkf --ic=CP932| head -n 1 > tmp.txt
		cat tmp.txt
		rm tmp.ts
		while read scramble onid tsid sid channel; do
			echo "- name: $channel" >> channels.yml
			echo "  type: GR" >> channels.yml
			echo "  channel: 'C${ch}'" >> channels.yml
		done < tmp.txt
		rm tmp.txt
	fi
done
for ch in `seq 1 2 23`; do
	for cl in `seq 0 2`; do
		cz=`printf %02d $ch`
		echo "********** BS${cz}_${cl} **********"
		recisdb tune --device /dev/px4video1 -t 4 -c BS${cz}_${cl} ./tmp.ts
		if [ -f tmp.ts ]; then
			./tschput tmp.ts | nkf --ic=CP932 > tmp.txt
			cat tmp.txt
			rm tmp.ts
			while read scramble onid tsid sid channel; do
				echo "- name: $channel" >> channels.yml
				echo "  type: BS" >> channels.yml
				echo "  channel: BS${cz}_${cl}" >> channels.yml
				echo "  serviceId: $sid" >> channels.yml
			done < tmp.txt
			rm tmp.txt
		fi
	done
done
for ch in `seq 2 2 24`; do
	echo "********** CS${ch} **********"
	recisdb tune --device /dev/px4video0 -t 4 -c CS${ch} ./tmp.ts
	if [ -f tmp.ts ]; then
		./tschput tmp.ts | nkf --ic=CP932 > tmp.txt
		cat tmp.txt
		rm tmp.ts
		while read scramble onid tsid sid channel; do
			echo "- name: $channel" >> channels.yml
			echo "  type: CS" >> channels.yml
			echo "  channel: CS${ch}" >> channels.yml
			echo "  serviceId: $sid" >> channels.yml
		done < tmp.txt
		rm tmp.txt
	fi
done
