double ctok(double c){
	return c+273.15;
}

double ctof(double c){
	return c*9/5+32;
}

double ktoc(double k){
	return k-273.15;
}

double ktof(double k){
	return ctof(ktoc(k));
}

double ftoc(double f){
	return (f-32)*5/9;
}

double ftok(double f){
	return ctok(ftoc(f));
}
