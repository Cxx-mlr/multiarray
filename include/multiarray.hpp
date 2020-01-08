#ifndef MULTIARRAY_HPP
#define MULTIARRAY_HPP

#include <iostream>

#include <type_traits>
#include <tuple>
#include <utility>

#define CX_BEGIN namespace cx {
#define CX_END }

CX_BEGIN
namespace detail {
	template <auto v>
	using identity = std::size_t;

	template <std::size_t... ns, std::size_t... indexes>
	constexpr std::size_t product(std::index_sequence <indexes...>) noexcept {
		std::tuple dims{ ns... };
		return (std::get <sizeof...(ns) - indexes - 1>(dims) * ... * 1);
	}

	template <std::size_t... ns, std::size_t... steps>
	constexpr std::size_t to_linear_index_impl(std::index_sequence <steps...>, identity <ns>... indexes) noexcept {
		return ((indexes * product <ns...>(std::make_index_sequence <sizeof...(ns) - steps - 1>())) + ...);
	}

	template <std::size_t... ns, std::size_t... indexes>
	constexpr auto mod_div(std::index_sequence <indexes...>, std::size_t index) noexcept {
		std::tuple dims{ ns... };

		((index /= std::get <sizeof...(ns) - 1U - indexes>(dims)), ...);

		if constexpr (sizeof...(indexes) == 0) {
			index %= std::get <sizeof...(ns) - 1>(dims);
		}

		else if constexpr (sizeof...(indexes) != sizeof...(ns) - 1U) {
			index %= std::get <sizeof...(indexes)>(dims);
		}

		return index;
	}

	template <std::size_t... ns, std::size_t... indexes>
	constexpr auto to_subscript_impl(std::index_sequence <indexes...>, std::size_t index) noexcept {
		return std::tuple{ mod_div <ns...>(std::make_index_sequence <sizeof...(ns) - 1U - indexes>(), index)... };
	}
}

template <class t, std::size_t n, std::size_t... ns>
class multiarray {
public:
	using value_type = t;
	using size_type = std::size_t;
	using pointer = t*;
	using const_pointer = const t*;
	using reference = t&;
	using const_reference = const t&;

	constexpr static std::size_t size() noexcept {
		return (ns * ... * n);
	}

	constexpr void fill(const t& value) {
		std::fill_n(zelems, size(), value);
	}

	constexpr void swap(multiarray& m) noexcept(std::is_nothrow_swappable_v <t>) {
		std::swap(zelems, m.zelems);
	}

	[[nodiscard]] constexpr auto begin() noexcept {
		return std::begin(zelems);
	}

	[[nodiscard]] constexpr auto end() noexcept {
		return std::end(zelems);
	}

	[[nodiscard]] constexpr auto rbegin() noexcept {
		return std::rbegin(zelems);
	}

	[[nodiscard]] constexpr auto rend() noexcept {
		return std::rend(zelems);
	}

	[[nodiscard]] constexpr auto cbegin() const noexcept {
		return std::cbegin(zelems);
	}

	[[nodiscard]] constexpr auto cend() const noexcept {
		return std::cend(zelems);
	}

	[[nodiscard]] constexpr auto crbegin() const noexcept {
		return std::crbegin(zelems);
	}

	[[nodiscard]] constexpr auto crend() const noexcept {
		return std::crend(zelems);
	}

	[[nodiscard]] constexpr t* data() noexcept {
		return zelems;
	}

	[[nodiscard]] constexpr const t* data() const noexcept {
		return zelems;
	}

	[[nodiscard]] constexpr reference front() noexcept {
		return zelems[0];
	}

	[[nodiscard]] constexpr const_reference front() const noexcept {
		return zelems[0];
	}

	[[nodiscard]] constexpr reference back() noexcept {
		return zelems[size() - 1];
	}

	[[nodiscard]] constexpr const_reference back() const noexcept {
		return zelems[size() - 1];
	}

	[[nodiscard]] constexpr reference operator()(std::size_t index, detail::identity <ns>... indexes) noexcept {
		return (*this)[to_linear_index(index, indexes...)];
	}

	[[nodiscard]] constexpr const_reference operator()(std::size_t index, detail::identity <ns>... indexes) const noexcept {
		return (*this)[to_linear_index(index, indexes...)];
	}

	[[nodiscard]] constexpr reference operator()(std::tuple <std::size_t, detail::identity <ns>...> tupl) noexcept {
		return std::apply([this](auto... indexes) -> t& {
			return (*this)[to_linear_index(indexes...)];
		}, tupl);
	}

	[[nodiscard]] constexpr const_reference operator()(std::tuple <std::size_t, detail::identity <ns>...> tupl) const noexcept {
		return std::apply([this](auto... indexes) -> const t& {
			return (*this)[to_linear_index(indexes...)];
		}, tupl);
	}

	[[nodiscard]] constexpr reference at(std::size_t index, detail::identity <ns>... indexes) {
		if (index >= n || ((indexes >= ns) || ...)) {
			throw std::range_error("array subscript out of range");
		}

		return (*this)[to_linear_index(index, indexes...)];
	}

	[[nodiscard]] constexpr const_reference at(std::size_t index, detail::identity <ns>... indexes) const {
		if (index >= n || ((indexes >= ns) || ...)) {
			throw std::range_error("array subscript out of range");
		}

		return (*this)[to_linear_index(index, indexes...)];
	}

	[[nodiscard]] constexpr reference at(std::tuple <std::size_t, detail::identity <ns>...> tupl) {
		return std::apply([this](auto... indexes) -> t& {
			return this->at(indexes...);
		}, tupl);
	}

	[[nodiscard]] constexpr const_reference at(std::tuple <std::size_t, detail::identity <ns>...> tupl) const {
		return std::apply([this](auto... indexes) -> const t& {
			return this->at(indexes...);
		}, tupl);
	}

	[[nodiscard]] constexpr reference operator[](std::size_t index) noexcept {
		return zelems[index];
	}

	[[nodiscard]] constexpr const_reference operator[](std::size_t index) const noexcept {
		return zelems[index];
	}

	[[nodiscard]] constexpr reference min() noexcept {
		t* min = &zelems[0];

		for (std::size_t idx = 1; idx < size(); ++idx) {
			if (zelems[idx] < *min) {
				min = &zelems[idx];
			}
		}

		return *min;
	}

	[[nodiscard]] constexpr const_reference min() const noexcept {
		const t* min = &zelems[0];

		for (std::size_t idx = 1; idx < size(); ++idx) {
			if (zelems[idx] < *min) {
				min = &zelems[idx];
			}
		}

		return *min;
	}

	[[nodiscard]] constexpr reference max() noexcept {
		t* max = &zelems[0];

		for (std::size_t idx = 1; idx < size(); ++idx) {
			if (zelems[idx] > * max) {
				max = &zelems[idx];
			}
		}

		return *max;
	}

	[[nodiscard]] constexpr const_reference max() const noexcept {
		const t* max = &zelems[0];

		for (std::size_t idx = 1; idx < size(); ++idx) {
			if (zelems[idx] > * max) {
				max = &zelems[idx];
			}
		}

		return *max;
	}

	[[nodiscard]] constexpr t sum() const noexcept {
		t result = zelems[0];

		for (std::size_t idx = 1; idx < size(); ++idx) {
			result += zelems[idx];
		}
	}

#define ARITHMM(OP)\
	for (std::size_t idx = 0; idx < size(); ++idx) {\
		zelems[idx] OP;\
	}\
	return *this;

#define ARITHMF(OP)\
	for (std::size_t idx = 0; idx < m.size(); ++idx) {\
		m.zelems[idx] OP;\
	}\
	return m;

	constexpr multiarray& operator++ () noexcept {
		for (std::size_t idx = 0; idx < size(); ++idx) {
			++zelems[idx];
		}

		return *this;
	}

	constexpr multiarray operator++ (int) noexcept {
		multiarray m = *this;

		for (std::size_t idx = 0; idx < size(); ++idx) {
			zelems[idx]++;
		}

		return m;
	}

	constexpr multiarray& operator-- () noexcept {
		for (std::size_t idx = 0; idx < size(); ++idx) {
			--zelems[idx];
		}

		return *this;
	}

	constexpr multiarray operator-- (int) noexcept {
		multiarray m = *this;

		for (std::size_t idx = 0; idx < size(); ++idx) {
			zelems[idx]--;
		}

		return m;
	}

	constexpr multiarray operator+ () noexcept {
		multiarray m;

		ARITHMF(= +(*this)[idx]);
	}

	constexpr multiarray operator- () noexcept {
		multiarray m;

		ARITHMF(= -(*this)[idx]);
	}

	constexpr multiarray operator~ () noexcept {
		multiarray m;

		ARITHMF(= ~(*this)[idx]);
	}

	constexpr multiarray operator! () noexcept {
		multiarray m;

		ARITHMF(= !(*this)[idx]);
	}

	constexpr multiarray& operator*= (const t& value) noexcept {
		ARITHMM(*= value);
	}

	constexpr multiarray& operator/= (const t& value) noexcept {
		ARITHMM(/= value);
	}

	constexpr multiarray& operator%= (const t& value) noexcept {
		ARITHMM(%= value);
	}

	constexpr multiarray& operator+= (const t& value) noexcept {
		ARITHMM(+= value);
	}

	constexpr multiarray& operator-= (const t& value) noexcept {
		ARITHMM(-= value);
	}

	constexpr multiarray& operator^= (const t& value) noexcept {
		ARITHMM(^= value);
	}

	constexpr multiarray& operator|= (const t& value) noexcept {
		ARITHMM(|= value);
	}

	constexpr multiarray& operator&= (const t& value) noexcept {
		ARITHMM(&= value);
	}

	constexpr multiarray& operator<<= (const t& value) noexcept {
		ARITHMM(<<= value);
	}

	constexpr multiarray& operator>>= (const t& value) noexcept {
		ARITHMM(>>= value);
	}

	constexpr multiarray& operator*= (const multiarray& m) noexcept {
		ARITHMM(*= m[idx]);
	}

	constexpr multiarray& operator/= (const multiarray& m) noexcept {
		ARITHMM(/= m[idx]);
	}

	constexpr multiarray& operator%= (const multiarray& m) noexcept {
		ARITHMM(%= m[idx]);
	}

	constexpr multiarray& operator+= (const multiarray& m) noexcept {
		ARITHMM(+= m[idx]);
	}

	constexpr multiarray& operator-= (const multiarray& m) noexcept {
		ARITHMM(-= m[idx]);
	}

	constexpr multiarray& operator^= (const multiarray& m) noexcept {
		ARITHMM(^= m[idx]);
	}

	constexpr multiarray& operator|= (const multiarray& m) noexcept {
		ARITHMM(|= m[idx]);
	}

	constexpr multiarray& operator&= (const multiarray& m) noexcept {
		ARITHMM(&= m[idx]);
	}

	constexpr multiarray& operator<<= (const multiarray& m) noexcept {
		ARITHMM(<<= m[idx]);
	}

	constexpr multiarray& operator>>= (const multiarray& m) noexcept {
		ARITHMM(>>= m[idx]);
	}

	[[nodiscard]] constexpr friend auto operator* (cx::multiarray <t, n, ns...> m, const t& value) noexcept {
		ARITHMF(*= value);
	}

	[[nodiscard]] constexpr friend auto operator* (const t& value, cx::multiarray <t, n, ns...> m) noexcept {
		ARITHMF(*= value);
	}

	[[nodiscard]] constexpr friend auto operator/ (cx::multiarray <t, n, ns...> m, const t& value) noexcept {
		ARITHMF(/= value);
	}

	[[nodiscard]] constexpr friend auto operator/ (const t& value, cx::multiarray <t, n, ns...> m) noexcept {
		ARITHMF(/= value);
	}

	[[nodiscard]] constexpr friend auto operator% (cx::multiarray <t, n, ns...> m, const t& value) noexcept {
		ARITHMF(%= value);
	}

	[[nodiscard]] constexpr friend auto operator% (const t& value, cx::multiarray <t, n, ns...> m) noexcept {
		ARITHMF(%= value);
	}

	[[nodiscard]] constexpr friend auto operator+ (cx::multiarray <t, n, ns...> m, const t& value) noexcept {
		ARITHMF(+= value);
	}

	[[nodiscard]] constexpr friend auto operator+ (const t& value, cx::multiarray <t, n, ns...> m) noexcept {
		ARITHMF(+= value);
	}

	[[nodiscard]] constexpr friend auto operator- (cx::multiarray <t, n, ns...> m, const t& value) noexcept {
		ARITHMF(-= value);
	}

	[[nodiscard]] constexpr friend auto operator- (const t& value, cx::multiarray <t, n, ns...> m) noexcept {
		ARITHMF(-= value);
	}

	[[nodiscard]] constexpr friend auto operator^ (cx::multiarray <t, n, ns...> m, const t& value) noexcept {
		ARITHMF(^= value);
	}

	[[nodiscard]] constexpr friend auto operator^ (const t& value, cx::multiarray <t, n, ns...> m) noexcept {
		ARITHMF(^= value);
	}

	[[nodiscard]] constexpr friend auto operator| (cx::multiarray <t, n, ns...> m, const t& value) noexcept {
		ARITHMF(|= value);
	}

	[[nodiscard]] constexpr friend auto operator| (const t& value, cx::multiarray <t, n, ns...> m) noexcept {
		ARITHMF(|= value);
	}

	[[nodiscard]] constexpr friend auto operator& (cx::multiarray <t, n, ns...> m, const t& value) noexcept {
		ARITHMF(&= value);
	}

	[[nodiscard]] constexpr friend auto operator& (const t& value, cx::multiarray <t, n, ns...> m) noexcept {
		ARITHMF(&= value);
	}

	[[nodiscard]] constexpr friend auto operator<< (cx::multiarray <t, n, ns...> m, const t& value) noexcept {
		ARITHMF(<<= value);
	}

	[[nodiscard]] constexpr friend auto operator>> (cx::multiarray <t, n, ns...> m, const t& value) noexcept {
		ARITHMF(>>= value);
	}

	[[nodiscard]] constexpr friend auto operator* (cx::multiarray <t, n, ns...> m, cx::multiarray <t, n, ns...> mm) noexcept {
		ARITHMF(*= mm[idx]);
	}

	[[nodiscard]] constexpr friend auto operator/ (cx::multiarray <t, n, ns...> m, cx::multiarray <t, n, ns...> mm) noexcept {
		ARITHMF(/= mm[idx]);
	}

	[[nodiscard]] constexpr friend auto operator% (cx::multiarray <t, n, ns...> m, cx::multiarray <t, n, ns...> mm) noexcept {
		ARITHMF(%= mm[idx]);
	}

	[[nodiscard]] constexpr friend auto operator+ (cx::multiarray <t, n, ns...> m, cx::multiarray <t, n, ns...> mm) noexcept {
		ARITHMF(+= mm[idx]);
	}

	[[nodiscard]] constexpr friend auto operator- (cx::multiarray <t, n, ns...> m, cx::multiarray <t, n, ns...> mm) noexcept {
		ARITHMF(-= mm[idx]);
	}

	[[nodiscard]] constexpr friend auto operator^ (cx::multiarray <t, n, ns...> m, cx::multiarray <t, n, ns...> mm) noexcept {
		ARITHMF(^= mm[idx]);
	}

	[[nodiscard]] constexpr friend auto operator| (cx::multiarray <t, n, ns...> m, cx::multiarray <t, n, ns...> mm) noexcept {
		ARITHMF(|= mm[idx]);
	}

	[[nodiscard]] constexpr friend auto operator& (cx::multiarray <t, n, ns...> m, cx::multiarray <t, n, ns...> mm) noexcept {
		ARITHMF(&= mm[idx]);
	}

	[[nodiscard]] constexpr friend auto operator<< (cx::multiarray <t, n, ns...> m, cx::multiarray <t, n, ns...> mm) noexcept {
		ARITHMF(<<= mm[idx]);
	}

	[[nodiscard]] constexpr friend auto operator>> (cx::multiarray <t, n, ns...> m, cx::multiarray <t, n, ns...> mm) noexcept {
		ARITHMF(>>= mm[idx]);
	}

	[[nodiscard]] constexpr static bool index_out_of_bounds(std::size_t index, detail::identity <ns>... indexes) noexcept {
		return index >= n || ((indexes >= ns) || ...);
	}

	[[nodiscard]] constexpr static bool index_out_of_bounds(std::tuple <std::size_t, detail::identity <ns>...> tupl) noexcept {
		return std::apply([](auto... indexes) {
			return multiarray::index_out_of_bounds(indexes...);
		}, tupl);
	}

	[[nodiscard]] constexpr static std::size_t to_linear_index(std::size_t index, detail::identity <ns>... indexes) noexcept {
		return detail::to_linear_index_impl <n, ns...>(std::make_index_sequence <sizeof...(ns) + 1>(), index, indexes...);
	}

	[[nodiscard]] constexpr static std::size_t to_linear_index(std::tuple <std::size_t, detail::identity <ns>...> tupl) noexcept {
		return std::apply([](auto... indexes) {
			return multiarray::to_linear_index(indexes...);
		}, tupl);
	}

	[[nodiscard]] constexpr static auto to_subscript(std::size_t index) noexcept {
		return detail::to_subscript_impl <n, ns...>(std::make_index_sequence <sizeof...(ns) + 1>(), index);
	}

	inline friend std::ostream& operator<< (std::ostream& out, const multiarray& m) {
		for (std::size_t idx = 0; idx < m.size(); ++idx) {
			std::cout << m.zelems[idx] << ' ';
		}

		return out;
	}

	t zelems[size()];
};
CX_END

#endif
