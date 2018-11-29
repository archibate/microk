{
	if ($2 && $2 < 700000) {
		ms = $2/1000;
		printf "%s: %5.1f\n", $1, ms;
		sum[$1] += ms;
		sum2[$1] += ms * ms;
		nr[$1] += 1;
	}
}
END {
	for (x in sum) {
		avg = sum[x]/nr[x];
		std = nr[x] == 1 ? 0 : sqrt(sum[x]*sum[x]-sum2[x])/(nr[x]-1);
		printf "[31;1m%2s: %3d * %5.1f / %5.1f[0m\n", x, nr[x], avg, std;
	}
}
