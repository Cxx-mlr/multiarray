#ifndef MULTIARRAY_HPP
#define MULTIARRAY_HPP

#include <iostream>
#include <array>

#include <type_traits>
#include <tuple>
#include <utility>

#define CX_BEGIN namespace cx {
#define CX_END }

CX_BEGIN
namespace detail {
	template <class t, std::size_t n, std::size_t... ns>
	struct multiarray_t {
		using type = std::array <typename multiarray_t <t, ns...>::type, n>;
	};

	template <class t, std::size_t n>
	struct multiarray_t <t, n> {
		using type = std::array <t, n>;
	};

	template <auto v>
	using identity = decltype(v);

	template <class t, class ptr_f>
	constexpr void for_each(t& value, ptr_f f) noexcept {
		f(value);
	}

	template <class t, std::size_t n, class ptr_f>
	constexpr void for_each(std::array <t, n>& arr, ptr_f f) noexcept {
		for (auto& x : arr) {
			for_each(x, f);
		}
	}

	template <class t, std::size_t n, class ptr_f>
	constexpr void for_each(const std::array <t, n>& arr, ptr_f f) noexcept {
		for (auto& x : arr) {
			for_each(x, f);
		}
	}

	template <class t, std::size_t n, class... size_t>
	constexpr auto& at_index(const std::array <t, n>& arr, std::size_t index, size_t... indexes) {
		if constexpr (sizeof...(indexes) == 0) {
			return arr[index];
		}

		else {
			return at_index(arr[index], indexes...);
		}
	}

	template <class t, std::size_t n, class... size_t>
	constexpr auto& at_index(std::array <t, n>& arr, std::size_t index, size_t... indexes) {
		if constexpr (sizeof...(indexes) == 0) {
			return arr[index];
		}

		else {
			return at_index(arr[index], indexes...);
		}
	}

	template <class t, std::size_t n, class... size_t>
	constexpr auto& at_index_noexcept(const std::array <t, n>& arr, std::size_t index, size_t... indexes) noexcept {
		if constexpr (sizeof...(indexes) == 0) {
			return arr[index];
		}

		else {
			return at_index_noexcept(arr[index], indexes...);
		}
	}

	template <class t, std::size_t n, class... size_t>
	constexpr auto& at_index_noexcept(std::array <t, n>& arr, std::size_t index, size_t... indexes) noexcept {
		if constexpr (sizeof...(indexes) == 0) {
			return arr[index];
		}

		else {
			return at_index_noexcept(arr[index], indexes...);
		}
	}

	template <auto, std::size_t v>
	inline constexpr std::size_t def_v = v;

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
	using multiarray_type = typename detail::multiarray_t <t, n, ns...>::type;

	using value_type = t;
	using size_type = std::size_t;
	using pointer = t*;
	using const_pointer = const t*;
	using reference = t&;
	using const_reference = const t&;

	constexpr static std::size_t size() noexcept {
		return (ns * ... * n);
	}

	constexpr void fill(const t& value) noexcept {
		detail::for_each(elems, [value](t& x) { x = value; });
	}

	constexpr void swap(multiarray& m) noexcept(std::is_nothrow_swappable_v <multiarray_type>) {
		elems.swap(m.elems);
	}

	[[nodiscard]] constexpr auto begin() noexcept {
		return elems.begin();
	}

	[[nodiscard]] constexpr auto end() noexcept {
		return elems.end();
	}

	[[nodiscard]] constexpr auto rbegin() noexcept {
		return elems.rbegin();
	}

	[[nodiscard]] constexpr auto rend() noexcept {
		return elems.rend();
	}

	[[nodiscard]] constexpr auto cbegin() const noexcept {
		return elems.cbegin();
	}

	[[nodiscard]] constexpr auto cend() const noexcept {
		return elems.cend();
	}

	[[nodiscard]] constexpr auto crbegin() const noexcept {
		return elems.crbegin();
	}

	[[nodiscard]] constexpr auto crend() const noexcept {
		return elems.crend();
	}

	[[nodiscard]] constexpr reference operator()(std::size_t index, detail::identity <ns>... indexes) noexcept {
		return detail::at_index_noexcept(elems, index, indexes...);
	}

	[[nodiscard]] constexpr const_reference operator()(std::size_t index, detail::identity <ns>... indexes) const noexcept {
		return detail::at_index_noexcept(elems, index, indexes...);
	}

	[[nodiscard]] constexpr reference operator()(std::tuple <std::size_t, detail::identity <ns>...> tupl) noexcept {
		return std::apply([this](auto... indexes) -> t& {
			return this->operator()(indexes...);
		}, tupl);
	}

	[[nodiscard]] constexpr const_reference operator()(std::tuple <std::size_t, detail::identity <ns>...> tupl) const noexcept {
		return std::apply([this](auto... indexes) -> const t& {
			return this->operator()(indexes...);
		}, tupl);
	}

	[[nodiscard]] constexpr reference at(std::size_t index, detail::identity <ns>... indexes) {
		if (index >= n || ((indexes >= ns) || ...)) {
			throw std::range_error("array subscript out of range");
		}

		return detail::at_index(elems, index, indexes...);
	}

	[[nodiscard]] constexpr const_reference at(std::size_t index, detail::identity <ns>... indexes) const {
		if (index >= n || ((indexes >= ns) || ...)) {
			throw std::range_error("array subscript out of range");
		}

		return detail::at_index(elems, index, indexes...);
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
		auto tupl = to_subscript(index);

		return std::apply([this](auto... indexes) -> t& {
			return this->operator()(indexes...);
		}, tupl);
	}

	[[nodiscard]] constexpr const_reference operator[](std::size_t index) const noexcept {
		auto tupl = to_subscript(index);

		return std::apply([this](auto... indexes) -> const t& {
			return this->operator()(indexes...);
		}, tupl);
	}

	[[nodiscard]] constexpr auto as_array() noexcept {
		std::array <t*, (ns * ... * n)> result = {};
		std::size_t index = 0;

		detail::for_each(elems, [&](t& x) {
			result[index] = &x;

			++index;
		});

		return result;
	}

	[[nodiscard]] constexpr auto as_array() const noexcept {
		std::array <const t*, (ns * ... * n)> result = {};
		std::size_t index = 0;

		for_each(elems, [&](const t& x) {
			result[index] = &x;

			++index;
		});

		return result;
	}

	[[nodiscard]] constexpr reference min() noexcept {
		t* min = &detail::at_index_noexcept(elems, 0, detail::def_v <ns, 0>...);

		detail::for_each(elems, [&min](t& x) { if (x < *min) { min = &x; } });

		return *min;
	}

	[[nodiscard]] constexpr reference max() noexcept {
		t* max = &detail::at_index_noexcept(elems, 0, detail::def_v <ns, 0>...);

		detail::for_each(elems, [&max](t& x) { if (x > * max) { max = &x; } });

		return *max;
	}

	[[nodiscard]] constexpr const_reference min() const noexcept {
		const t* min = &detail::at_index_noexcept(elems, 0, detail::def_v <ns, 0>...);

		detail::for_each(elems, [&min](const t& x) { if (x < *min) { min = &x; } });

		return *min;
	}

	[[nodiscard]] constexpr const_reference max() const noexcept {
		const t* max = &detail::at_index_noexcept(elems, 0, detail::def_v <ns, 0>...);

		detail::for_each(elems, [&max](const t& x) { if (x > * max) { max = &x; } });

		return *max;
	}

	[[nodiscard]] constexpr t sum() const noexcept {
		t result = 0;

		detail::for_each(elems, [&](const t& x) {
			result += x;
		});

		return result;
	}

#define ARITHMM(OP)\
	std::size_t idx = 0;\
	detail::for_each(elems, [&](t& x) {\
		x OP;\
		++idx;\
	});\
	return *this;

#define ARITHMF(OP)\
	std::size_t idx = 0;\
	detail::for_each(m.elems, [&](t& x) {\
		x OP;\
		++idx;\
	});\
	return m;

	constexpr multiarray& operator++ () noexcept {
		detail::for_each(elems, [](t& x) { ++x; });

		return *this;
	}

	constexpr multiarray operator++ (int) noexcept {
		multiarray m = *this;

		detail::for_each(elems, [](t& x) { x++; });

		return m;
	}

	constexpr multiarray& operator-- () noexcept {
		detail::for_each(elems, [](t& x) { --x; });

		return *this;
	}

	constexpr multiarray operator-- (int) noexcept {
		multiarray m = *this;

		detail::for_each(elems, [](t& x) { x--; });

		return m;
	}

	constexpr multiarray operator+ () noexcept {
		multiarray m;
		
		ARITHMF(= +multiarray::operator[](idx));
	}

	constexpr multiarray operator- () noexcept {
		multiarray m;

		ARITHMF(= -multiarray::operator[](idx));
	}

	constexpr multiarray operator~ () noexcept {
		multiarray m;

		ARITHMF(= ~multiarray::operator[](idx));
	}

	constexpr multiarray operator! () noexcept {
		multiarray m;

		ARITHMF(= !multiarray::operator[](idx));
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
		detail::for_each(m.elems, [&](const t& x) { out << x << ' '; });

		return out;
	}

	multiarray_type elems;

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
};
CX_END

#endif
