#include "big_integer.h"
#include <vector>
#include <string>
#include <algorithm>


using namespace std;

unsigned int const MAX_DIGIT = std::numeric_limits<unsigned int>::max();
int const BASE = 32;
int const BASE_10_INTLEN = 9;
int const MAX_INT_LEN9 = 1000000000;

template <typename T>
unsigned long long ull_cast(T a) {
	return static_cast<unsigned long long>(a);
}

size_t big_integer::length() const{
	return data.size();
}

unsigned int big_integer::get_digit(size_t ind) const {
	if (ind < length()) {
		return data[ind];
	}
	return sign ? MAX_DIGIT : 0 ;
}

void big_integer::make_correct() {
	while ((length() > 0 && !sign && data.back() == 0 ) || (length() > 1 && sign && data.back() == MAX_DIGIT)) {
		data.pop_back();
	}
}

big_integer::big_integer() : sign(false) {}

big_integer::big_integer(big_integer const& other) : sign(other.sign), data(other.data) {
	make_correct();
}
big_integer::big_integer(bool new_sign, vector<unsigned int> const &new_data) : sign(new_sign), data(new_data) {
	make_correct();
}

void big_integer::copy(big_integer &other) noexcept {
	std::swap(data, other.data);
	std::swap(sign, other.sign);
}

big_integer::big_integer(int a) : sign(a < 0), data(1) {
    data[0] = a; 
	make_correct();
}

big_integer::big_integer(long long a) : sign(a < 0), data(2) {
	data[0] = a & MAX_DIGIT;
	data[1] = a >> BASE;
	make_correct();
}

int pow_10(int p) {
	return (p == 0) ? 1 : 10 * pow_10(p - 1);
}


big_integer string_to_big_integer(std::string const& str) {
	size_t begin, ind;
	begin = 0;
	if (str[0] == '-' || str[0] == '+') {
		begin++;
	}
	big_integer res(0);
	for (ind = begin + BASE_10_INTLEN; ind < str.size(); ind += BASE_10_INTLEN) {
		res = mul_big_short(res, MAX_INT_LEN9) + stoi(str.substr(ind - 9, 9));
	}
	string endstr = str.substr(ind - 9);
	res = mul_big_short(res, (pow_10(endstr.size()))) + stoi(endstr);
	return (str[0] == '-' ) ? -res : res;
}

big_integer::big_integer(std::string const& str) : big_integer(string_to_big_integer(str)) {}

big_integer& big_integer::operator=(big_integer const& other) {
	big_integer temp(other);
	copy(temp);
	return *this;
}

bool operator==(big_integer const& a, big_integer const& b) {
	if (a.sign == b.sign && a.data == b.data) {
		return true;
	}
	return false;
}

bool operator!=(big_integer const& a, big_integer const& b) {
	return !(a == b);
}
bool operator<(big_integer const& a, big_integer const& b) {
	if (a.sign != b.sign) {
		if (a.sign) {
			return true;
		}
		return false;
	}
	if (a.sign && a.length() > b.length()|| !a.sign && a.length() < b.length()) {
		return true;
	}
	if (a.sign && a.length() < b.length() || !a.sign && a.length() > b.length()) {
		return false;
	}
	size_t i = 0;
	while (i < a.length()) {
		if (a.get_digit(i) < b.get_digit(i)) {
			return true;
		}
		i++;
	}
	return false;
}
bool operator>(big_integer const& a, big_integer const& b) {
	return b < a;
}
bool operator<=(big_integer const& a, big_integer const& b) {
	return !(a > b);
}
bool operator>=(big_integer const& a, big_integer const& b) {
	return !(a < b);
}

big_integer big_integer::operator+() const {
	return *this;
}

big_integer big_integer::operator-() const {
	if (length() == 0) {
		return *this;
	}
	else {
		return(~*this + 1);
	}
}
big_integer big_integer::operator~() const {
	vector<unsigned int> temp(data.size());
	for (size_t i = 0; i < length(); i++) {
		temp[i] = ~data[i];
	}
	return big_integer(!sign, temp);
}
template<class FuncT>
big_integer bitwise_operation(big_integer a, big_integer const& b, FuncT f) {
	size_t len = max(a.length(), b.length());
	vector<unsigned int> temp(len);
	for (size_t i = 0; i < len; i++) {
		temp[i] = f(a.get_digit(i), b.get_digit(i));
	}
	return big_integer(f(a.sign, b.sign), temp);
}


big_integer operator&(big_integer a, big_integer const& b) {
	return bitwise_operation(a, b, [](unsigned int x, unsigned int y) {return x & y; });
}
big_integer operator|(big_integer a, big_integer const& b) {
	return bitwise_operation(a, b, [](unsigned int x, unsigned int y) {return x | y; });
}
big_integer operator^(big_integer a, big_integer const& b) {
	return bitwise_operation(a, b, [](unsigned int x, unsigned int y) {return x ^ y; });
}
big_integer& big_integer::operator&=(big_integer const& rhs) {
	return *this = *this & rhs;
}
big_integer& big_integer::operator|=(big_integer const& rhs) {
	return *this = *this | rhs;
}
big_integer& big_integer::operator^=(big_integer const& rhs) {
	return *this = *this ^ rhs;
}


big_integer abs(big_integer const& a) {
	return (a.sign ? -a : a);
}


big_integer operator<<(big_integer a, int b) {
	vector<unsigned int> res(a.length() + 1);
	unsigned int carry = 0;
	for (size_t i = 0; i < a.length() + 1; i++) {
		res[i] = (a.get_digit(i) << b) + carry;
		carry = (a.get_digit(i) >> (BASE - b));
	}
	//res[a.length()] = carry;
	return big_integer(a.sign, res);
}
big_integer operator>>(big_integer a, int b) {
	vector<unsigned int> res(a.length());

	for (size_t i = 0; i < a.length(); i++) {
		res[i] = (a.get_digit(i) >> b) | (a.get_digit(i + 1) << (BASE - b));
	}
	return big_integer(a.sign, res);
}
big_integer& big_integer::operator<<=(int rhs) {
	return *this = *this << rhs;
}
big_integer& big_integer::operator>>=(int rhs) {
	return *this = *this >> rhs;
}

big_integer operator+(big_integer a, big_integer const& b) {
	long long carry = 0;
	long long sum;
	long long MAXD = MAX_DIGIT;
	size_t len = max(a.length(), b.length()) + 2;
	vector<unsigned int> res(len);
	for (size_t i = 0; i < len; i++) {
		sum = (carry + a.get_digit(i)) + b.get_digit(i);
		carry = sum >> BASE;
		res[i] = sum & MAX_DIGIT;
	}
	return big_integer(res.back(), res);
}
big_integer& big_integer::operator+=(big_integer const& rhs) {
	return *this  = *this + rhs;
}
big_integer& big_integer::operator++() {
	return *this = *this + 1;
}
big_integer big_integer::operator++(int) {
	big_integer r = *this;
	++*this;
	return r;
}

big_integer operator-(big_integer a, big_integer const& b) {
	return a + (-b);
}
big_integer& big_integer::operator-=(big_integer const& rhs) {
	return *this = *this - rhs;
}
big_integer& big_integer::operator--() {
	return *this = *this - 1;
}
big_integer big_integer::operator--(int) {
	big_integer r = *this;
	--*this;
	return r;
}

big_integer mul_big_short(big_integer a, unsigned int const& b) {
	vector<unsigned int> res(a.length() + 1);
	unsigned long long carry = 0;
	unsigned long long MAXD = MAX_DIGIT;
	unsigned long long mul = 0;
	for (size_t i = 0; i < a.length(); i++) {
		mul = ull_cast(a.get_digit(i)) * b + carry;
		res[i] = (mul & MAX_DIGIT);
		carry = mul >> BASE;
	}
	res[a.length()] = carry & MAX_DIGIT;
	return big_integer(0, res);
}
big_integer operator*(big_integer a, big_integer const& b) {
	big_integer res(0);
	big_integer abs_a = abs(a);
	big_integer abs_b = abs(b);
	for (size_t i = 0; i < abs_b.length(); i++) {
		res.data.insert(res.data.begin(), 0);
		//res = ((res << 1) << 31);
		res += mul_big_short(abs_a, abs_b.get_digit(abs_b.length() - 1 - i));
	}
	if (a.sign ^ b.sign) return -res;
	else return res;
}
big_integer& big_integer::operator*=(big_integer const& rhs) {
	return *this = *this * rhs;
}

big_integer div_big_short(big_integer a, unsigned int const& b) {
	vector<unsigned int> res(a.length());
	unsigned long long carry = 0;
	unsigned long long temp = 0;
	unsigned long long MAXD = MAX_DIGIT;
	size_t len = a.length();
	for (size_t i = 0; i < len; i++) {
		temp = (carry *  (MAXD + 1)) + a.get_digit(len - i - 1);
		res[len - i - 1] = temp / ull_cast(b);
		carry = temp % b;
	}
	return big_integer(0, res);
}
unsigned int trial(unsigned int r1, unsigned int r2, unsigned int d1) {
	unsigned long long x, res;
	unsigned long long y = d1;
	x = (ull_cast(r1) << BASE) + r2;
	res = (x / y);
	return min((unsigned int)res, MAX_DIGIT);
}
bool cmp_prefix(big_integer& r, big_integer& dq, size_t k, size_t m) {
	size_t i = m, j = 0;
	while (i != j) {
		if (r.get_digit(i + k) != dq.get_digit(i)) {
			j = i;
		} else {
			--i;
		}
	}
	return r.get_digit(i + k) < dq.get_digit(i);
}
void difference(big_integer& r, big_integer & dq, size_t k, size_t m) {
	unsigned long long borrow = 0, diff, b = ull_cast(MAX_DIGIT) + 1;
	for (size_t i = 0; i <= m; i++) {
		diff = ull_cast(r.get_digit(i + k)) - ull_cast(dq.get_digit(i)) - borrow + b;
		r.data[i + k] = diff % b;
		borrow = 1 - diff / b;
	}
}
pair<big_integer, big_integer> longdivide(big_integer const& x, big_integer const & y) {
	unsigned int f = (ull_cast(MAX_DIGIT) + 1) / ull_cast((y.get_digit(y.length() - 1) + 1));
	big_integer r, q, d, dq;
	r = mul_big_short(x, f);
	d = mul_big_short(y, f);
	size_t m = y.length(), n = x.length();
	r.data.push_back(0);
	q.data.resize(n - m + 1);
	for (size_t i = 0; i < n - m + 1; i++) {
		size_t k = n - m - i;
		unsigned int qt = trial(r.get_digit(k + m), r.get_digit(k + m - 1), d.get_digit(m - 1));
		dq = mul_big_short(d, qt);
		while (cmp_prefix(r, dq, k, m)) {
			qt -= 1;
			dq -= d;
		}
		q.data[k] = qt;
		difference(r, dq, k, m);
	}
	r = div_big_short(r, f);
	return { q, r };
}


big_integer operator/(big_integer a, big_integer const& b) {
	if (b == 0) {
		throw(std::runtime_error("Division by zero!"));
	}
	if (b.length() == 1) {
		if (a.sign ^ !b.sign) {
			return div_big_short(abs(a), abs(b).get_digit(0));
		}
		else {
			return -div_big_short(abs(a), abs(b).get_digit(0));
		}
	}
	if (a.length() < b.length()) {
		return 0;
	}
	if (a == b) {
		return 1;
	}
	if (a.sign ^ !b.sign) {
		return longdivide(abs(a), abs(b)).first;
	}
	else {
		return -longdivide(abs(a), abs(b)).first;
	}
}
big_integer operator%(big_integer a, big_integer const& b) {
	if (b == 0) {
		throw(std::runtime_error("Division by zero!"));
	}
	if (b.length() == 1) {
		if (a.sign ^ !b.sign) {
			return a - mul_big_short(div_big_short(abs(a), abs(b).get_digit(0)), abs(b).get_digit(0));
		}
		else {
			return a + div_big_short(abs(a), abs(b).get_digit(0)) * b;
		}
	}
	if (a.length() < b.length()) {
		return a;
	}
	if (a == b) {
		return 0;
	}
	if (a.sign ^ !b.sign) {
		return longdivide(abs(a), abs(b)).second;
	}
	else {
		return -longdivide(abs(a), abs(b)).second;
	}
}

big_integer& big_integer::operator/=(big_integer const& rhs) {
	return *this = *this / rhs;
}
big_integer& big_integer::operator%=(big_integer const& rhs) {
	return *this = *this % rhs;
}

string to_string(big_integer const& a) {
	string res = "";
	big_integer b = abs(a);
	big_integer c = b;
	while (b > 0) {
		b = c / MAX_INT_LEN9;
		unsigned int digit = (c - mul_big_short(b, MAX_INT_LEN9)).get_digit(0);
		string dig = to_string(digit);
		while (dig.size() < BASE_10_INTLEN && b > 0) {
			dig = "0" + dig;
		}
		res = dig + res;
		c = b;
	}
	if (a.sign) {
		res = "-" + res;
	}
	if (res.size() == 0) {
		res = "0";
	}
	return res;
}