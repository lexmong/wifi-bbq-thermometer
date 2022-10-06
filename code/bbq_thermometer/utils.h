/**
 * Celsius to Kelvin
 * c: degrees in Celsius
 */
double ctok(double c){
	return c+273.15;
}

/**
 * Celsius to Fahrenheit
 * c: degrees in Celsius
 */
double ctof(double c){
	return c*9/5+32;
}

/**
 * Kelvin to Celsius
 * k: degrees in Kelvin
 */
double ktoc(double k){
	return k-273.15;
}

/**
 * Kelvin to Fahrenheit
 * k: degrees in Kelvin
 */
double ktof(double k){
	return ctof(ktoc(k));
}

/**
 * Fahrenheit to Celsius
 * f: degrees in Fahrenheit
 */
double ftoc(double f){
	return (f-32)*5/9;
}

/**
 * Fahrenheit to Kelvin
 * f: degrees in Fahrenheit
 */
double ftok(double f){
	return ctok(ftoc(f));
}
