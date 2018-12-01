fr=${1?from what ext name}
to=${2?to what ext name}
for x in $(ls *.$fr -l | awk '{print $9}' | awk -F'.' '{print $1}'); do mv $x.$fr $x.$to; done
