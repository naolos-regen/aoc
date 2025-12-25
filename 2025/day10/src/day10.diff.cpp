#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<utility>
#include<cstdlib>
#include<climits>
#include<numeric>

auto inline parse(const std::string& line, std::vector<std::vector<int>>& buttons, std::vector<int>& power) -> void {
	// Parse part 1
	int i = 1;
	while(line[i] != ']'){
		++i;
	};
	int bit_length = i - 1;

	// Parse part 2
	i = line.find('(', i);
	int end;
	while(i != std::string::npos){
		++i;
		end = i;
		bool end_parenthesis = false;
		std::vector<int> button;
		button.reserve(15);
		while(!end_parenthesis){
			while(line[end] != ',' && line[end] != ')'){++end;};
			button.push_back(std::stoi(line.substr(i, end - i)));
			if(line[end] == ')'){end_parenthesis = true;};
			i = ++end;
		};
		i = line.find('(', i + 1);
		buttons.push_back(button);
	};

	// Parse part 3
	power.reserve(bit_length);
	i = line.find('{', end) + 1;
	end = i;
	bool end_parenthesis = false;
	while(!end_parenthesis){
		while(line[end] != ',' && line[end] != '}'){++end;};
		power.push_back(std::stoi(line.substr(i, end - i)));
		if(line[end] == '}'){end_parenthesis = true;};
		i = ++end;
	};
};

auto inline swap_columns(std::vector<std::vector<int>>& A, int a, int b) -> void {
	if(a == b){return;};
	for(int r {}; r < A.size(); ++r){std::swap(A[r][a], A[r][b]);};
};

auto inline modified_gaussian_elimination(std::vector<std::vector<int>>& A) -> int {
	const int m = A.size(), n = A.front().size();
	int pivots {}, end = n - 2;
	bool pivot_exists = true;

	// Modified Row Echelon Form
	for(int row {}, pivot {}; row < m && pivot <= end && pivot_exists; ++row){
		// Find pivot and swap columns and rows
		auto& pivot_row = A[row];
		pivot_exists = pivot_row[pivot] != 0;
		while(!pivot_exists && pivot <= end){
			for(int r = row; !pivot_exists && r < m; ++r){
				if(A[r][pivot] != 0){
					std::swap(pivot_row, A[r]);
					pivot_exists = true;
				};
			};
			if(!pivot_exists){swap_columns(A, pivot, end--);};
		};

		if(pivot_exists){
			++pivots;
			// Make sure pivot positive
			if(pivot_row[pivot] < 0){
				for(int c = pivot; c < n; ++c){
					pivot_row[c] *= -1;
				};
			};

			// Make all values below pivot 0
			for(int r = row + 1; r < m; ++r){
				if(A[r][pivot] != 0){
					int standard = pivot_row[pivot] * A[r][pivot] / std::gcd(pivot_row[pivot], A[r][pivot]);
					int pivot_factor = standard / pivot_row[pivot];
					int row_factor = standard / A[r][pivot];
					for(int c = pivot; c < n; ++c){
						A[r][c] = A[r][c] * row_factor - pivot_row[c] * pivot_factor;
					};
				};
			};
			++pivot;
		};

	};
	// Modified Reduced Row Echelon Form
	for(int pivot = pivots - 1; pivot > 0; --pivot){
		const std::vector<int>& pivot_row = A[pivot];
		for(int r = pivot - 1; r > -1; --r){
			int candidate = A[r][pivot];
			if(candidate != 0){
				int standard = pivot_row[pivot] * candidate / std::gcd(pivot_row[pivot], A[r][pivot]);
				int pivot_factor = standard / pivot_row[pivot];
				int candidate_factor = standard / A[r][pivot];
				for(int c = r; c < n; ++c){
					A[r][c] = A[r][c] * candidate_factor - pivot_row[c] * pivot_factor;
				};
			};
		};
	};
	return pivots;
};

auto inline unique_back_substitution(const int n, const std::vector<std::vector<int>>& A, const int pivots) -> int {
	int total {};
	std::vector<int> X(A.front().size() - 1, 0);
	for(int pivot = pivots - 1; pivot > -1; --pivot){
		const std::vector<int>& pivot_row = A[pivot];
		const int factor = pivot_row[pivot];
		int res = pivot_row[n - 1];
		res /= factor;
		X[pivot] = res;
		total += res;
	};
	return total;
};

auto inline ceil_div(int a, int b) -> int {
	return a % b == 0 ? a / b : a / b + 1;
};
	

auto inline get_updated_bounds(const int n, const std::vector<std::vector<int>>& A, const int pivots, const std::vector<int>& X, const std::vector<std::pair<int, std::pair<int,int>>>& free_vars, int free_var_ptr) -> std::pair<int,int> {
	int left = free_vars[free_var_ptr].second.first, right = free_vars[free_var_ptr].second.second;
	const int x_var = free_vars[free_var_ptr].first;
	for(int r = pivots - 1; r > -1; --r){
		if(A[r][x_var] != 0){
			const int factor = A[r][x_var];
			int res = A[r][n - 1];
			bool can_calculate_inequality = true;
			for(int c = pivots; c < n - 1 && can_calculate_inequality; ++c){
				if(c != x_var && A[r][c] != 0){
					if(X[c] > -1){
						res -= X[c] * A[r][c];
					} else {
						can_calculate_inequality = false;
					};
				};
			};
			if(can_calculate_inequality){
				if(factor > 0){
					res /= factor;
					if(res < right){right = res;};
				} else {
					res = ceil_div(res, factor);
					if(res > left){left = res;};
				};
			};
		};
	};
	return {left, right};
};

auto inline get_init_bounds(const int n, const std::vector<std::vector<int>>& A, const int pivots, const int x_var, const int max_power) -> std::pair<int,int> {
	const int init_left = 0, init_right = max_power;
	std::pair<int,int> bounds {init_left, init_right};
	for(int r = pivots - 1; r > -1; --r){
		if(A[r][x_var] != 0){
			bool is_unique = true;
			for(int c = pivots; c < n - 1 && is_unique; ++c){if(c != x_var && A[r][c] != 0){is_unique = false;};};
			if(is_unique){
				const int factor = A[r][x_var];
				int res;
				if(factor > 0){
					res = A[r][n - 1] / factor;
					if(res < bounds.second){bounds.second = res;};
				} else {
					res = ceil_div(A[r][n - 1], factor);
					if(res > bounds.first){bounds.first = res;};
				};
			};
		};
	};
	return bounds;
};

auto inline dfs(const int n, const std::vector<std::vector<int>>& A, const int pivots, std::vector<int>& X, const std::vector<std::pair<int, std::pair<int,int>>>& free_vars, int free_var_ptr, int base) -> int {
	int leader = INT_MAX;
	int x_var = free_vars[free_var_ptr].first;
	std::pair<int,int> bounds = free_var_ptr > 0 ? get_updated_bounds(n,A,pivots,X,free_vars,free_var_ptr): free_vars[free_var_ptr].second;
	if(free_var_ptr == free_vars.size() - 1){
		for(int val = bounds.first; val < bounds.second + 1; ++val){
			X[x_var] = val;
			int total = base + val;
			bool is_valid = true;
			for(int r = pivots - 1; r > -1 && is_valid; --r){
				const int factor = A[r][r];
				int res = A[r][n - 1];
				for(int c = r + 1; c < n - 1; ++c){
					res -= A[r][c] * X[c];
				};
				int final_res = res / factor;
				// Make sure final result isn't a fraction and isn't negative
				if(res == final_res * factor && final_res > -1){
					X[r] = final_res;
					total += final_res;
				} else {is_valid = false;};
			};
			if(is_valid && total < leader){leader = total;};
		};
		X[x_var] = -1;
		return leader;
	};
	for(int val = bounds.first; val < bounds.second + 1; ++val){
		X[x_var] = val;
		int res = dfs(n, A, pivots, X, free_vars, free_var_ptr + 1, base + val);
		if(res < leader){leader = res;};
		X[x_var] = -1;
	};
	return leader;
};

auto inline multi_back_substitution(const int n, const std::vector<std::vector<int>>& A, const int pivots, const int max_power) -> int {
	int total {};

	// Get init bounds for free variables
	std::vector<std::pair<int, std::pair<int,int>>> free_vars;
	free_vars.reserve(3);
	auto comp = [](const std::pair<int,std::pair<int,int>>& a, std::pair<int,std::pair<int,int>>& b) -> bool {
		return (a.second.second - a.second.first) < (b.second.second - b.second.first);
	};
	for(int x_var = pivots; x_var < n - 1; ++x_var){
		free_vars.push_back({x_var, get_init_bounds(n, A, pivots, x_var, max_power)});
	};
	// Start With Free Variables with the Shortest bounds
	//std::qsort(free_vars.begin(), free_vars.end(), comp);
	std::vector<int> X(n - 1, -1);
	int init_free_var_ptr {}, init_x_var_total {};
	return dfs(n, A, pivots, X, free_vars, init_free_var_ptr, init_x_var_total);
};

auto main() -> int{
	std::string file_name = "res/input.txt";
	std::ifstream read_stream(file_name, std::ios::in);
	if(read_stream.is_open()){
		std::string line;
		int total {};
		while(std::getline(read_stream, line)){
			// Generate Augmented A matrix
			std::vector<std::vector<int>> buttons;
			std::vector<int> power;
			parse(line, buttons, power);
			const int m = power.size(), n = buttons.size() + 1;
			std::vector<std::vector<int>> A(m, std::vector<int>(n, 0));
			for(int c{}; c < n - 1; ++c){
				const std::vector<int>& button = buttons[c];
				for(const int r: button){
					A[r][c] = 1;
				};
			};
			// Get Default Upper Bound
			int max_power {};
			for(int r {}; r < m; ++r){int joltage = power[r]; A[r][n -1] = joltage; if(joltage > max_power){max_power = joltage;};};

			// Solve
			int pivots = modified_gaussian_elimination(A);
			int val = pivots == n - 1 ? unique_back_substitution(n, A, pivots): multi_back_substitution(n, A, pivots,max_power);
			total += val;
		};
		std::cout << "Result: " << total << " presses \n";
		read_stream.close();
	};
	return 0;
};
