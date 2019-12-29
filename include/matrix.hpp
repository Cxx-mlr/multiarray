#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <array>

#define CX_BEGIN namespace cx {
#define CX_END }

CX_BEGIN
namespace detail {
	template <class t, std::size_t n, std::size_t... ns>
	struct matrix_t {
		using type = std::array <typename matrix_t <t, ns...>::type, n>;
	};

	template <class t, std::size_t n>
	struct matrix_t <t, n> {
		using type = std::array <t, n>;
	};

	template <auto>
	struct pack { using type = std::size_t; };

	template <auto v>
	using pack_t = typename pack <v>::type;

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

	template <auto, std::size_t v>
	inline constexpr std::size_t def_v = v;
}

template <class t, std::size_t n, std::size_t... ns>
class matrix {
public:
	using matrix_type = typename detail::matrix_t <t, n, ns...>::type;

	using value_type = t;
	using size_type = std::size_t;
	using pointer = t*;
	using const_pointer = const t*;
	using reference = t&;
	using const_reference = const t&;

	constexpr void fill(const t& value) noexcept {
		for_each(elems, [value](t& x) { x = value; });
	}

	constexpr void swap(matrix_type& m) noexcept {
		elems.swap(m);
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

	[[nodiscard]] constexpr reference operator()(std::size_t index, detail::pack_t <ns>... indexes) noexcept {
		return detail::at_index(elems, index, indexes...);
	}

	[[nodiscard]] constexpr const_reference operator()(std::size_t index, detail::pack_t <ns>... indexes) const noexcept {
		return detail::at_index(elems, index, indexes...);
	}

	[[nodiscard]] constexpr reference at(std::size_t index, detail::pack_t <ns>... indexes) {
		if (index >= n || ((indexes >= ns) || ...)) {
			throw std::range_error("array subscript out of range");
		}

		return detail::at_index(elems, index, indexes...);
	}

	[[nodiscard]] constexpr const_reference at(std::size_t index, detail::pack_t <ns>... indexes) const {
		if (index >= n || ((indexes >= ns) || ...)) {
			throw std::range_error("array subscript out of range");
		}

		return detail::at_index(elems, index, indexes...);
	}

	[[nodiscard]] constexpr auto& operator[](std::size_t index) noexcept {
		return elems[index];
	}

	[[nodiscard]] constexpr auto& operator[](std::size_t index) const noexcept {
		return elems[index];
	}

	[[nodiscard]] constexpr reference min() noexcept {
		t* min = &detail::at_index(elems, 0, detail::def_v <ns, 0>...);

		detail::for_each(elems, [&min](t& x) { if (x < *min) { min = &x; } });

		return *min;
	}

	[[nodiscard]] constexpr reference max() noexcept {
		t* max = &detail::at_index(elems, 0, detail::def_v <ns, 0>...);

		detail::for_each(elems, [&max](t& x) { if (x > * max) { max = &x; } });

		return *max;
	}

	[[nodiscard]] constexpr const_reference min() const noexcept {
		const t* min = &detail::at_index(elems, 0, detail::def_v <ns, 0>...);

		detail::for_each(elems, [&min](const t& x) { if (x < *min) { min = &x; } });

		return *min;
	}

	[[nodiscard]] constexpr const_reference max() const noexcept {
		const t* max = &detail::at_index(elems, 0, detail::def_v <ns, 0>...);

		detail::for_each(elems, [&max](const t& x) { if (x > * max) { max = &x; } });

		return *max;
	}

#define ARITHMV(OP) detail::for_each(elems, [value](t& x) { x OP value; });

	constexpr matrix& operator*= (const t& value) noexcept {
		ARITHMV(*= );

		return *this;
	}

	constexpr matrix& operator/= (const t& value) noexcept {
		ARITHMV(/= );

		return *this;
	}

	constexpr matrix& operator%= (const t& value) noexcept {
		ARITHMV(%= );

		return *this;
	}

	constexpr matrix& operator+= (const t& value) noexcept {
		ARITHMV(+= );

		return *this;
	}

	constexpr matrix& operator-= (const t& value) noexcept {
		ARITHMV(-= );

		return *this;
	}

	constexpr matrix& operator^= (const t& value) noexcept {
		ARITHMV(^= );

		return *this;
	}

	constexpr matrix& operator|= (const t& value) noexcept {
		ARITHMV(|= );

		return *this;
	}

	constexpr matrix& operator&= (const t& value) noexcept {
		ARITHMV(&= );

		return *this;
	}

	constexpr matrix& operator<<= (const t& value) noexcept {
		ARITHMV(<<= );

		return *this;
	}

	constexpr matrix& operator>>= (const t& value) noexcept {
		ARITHMV(>>= );

		return *this;
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

	inline friend std::ostream& operator<< (std::ostream& out, const matrix& m) {
		detail::for_each(m.elems, [&](const t& x) { out << x << ' '; });

		return out;
	}

	matrix_type elems;

	[[nodiscard]] constexpr friend auto operator* (cx::matrix <t, n, ns...> m, const t& value) noexcept {
		auto& elems = m.elems;
		ARITHMV(*= );

		return m;
	}

	[[nodiscard]] constexpr friend auto operator* (const t& value, cx::matrix <t, n, ns...> m) noexcept {
		auto& elems = m.elems;
		ARITHMV(*= );

		return m;
	}

	[[nodiscard]] constexpr friend auto operator/ (cx::matrix <t, n, ns...> m, const t& value) noexcept {
		auto& elems = m.elems;
		ARITHMV(/= );

		return m;
	}

	[[nodiscard]] constexpr friend auto operator/ (const t& value, cx::matrix <t, n, ns...> m) noexcept {
		auto& elems = m.elems;
		ARITHMV(/= );

		return m;
	}

	[[nodiscard]] constexpr friend auto operator% (cx::matrix <t, n, ns...> m, const t& value) noexcept {
		auto& elems = m.elems;
		ARITHMV(%= );

		return m;
	}

	[[nodiscard]] constexpr friend auto operator% (const t& value, cx::matrix <t, n, ns...> m) noexcept {
		auto& elems = m.elems;
		ARITHMV(%= );

		return m;
	}

	[[nodiscard]] constexpr friend auto operator+ (cx::matrix <t, n, ns...> m, const t& value) noexcept {
		auto& elems = m.elems;
		ARITHMV(+= );

		return m;
	}

	[[nodiscard]] constexpr friend auto operator+ (const t& value, cx::matrix <t, n, ns...> m) noexcept {
		auto& elems = m.elems;
		ARITHMV(+= );

		return m;
	}

	[[nodiscard]] constexpr friend auto operator- (cx::matrix <t, n, ns...> m, const t& value) noexcept {
		auto& elems = m.elems;
		ARITHMV(-= );

		return m;
	}

	[[nodiscard]] constexpr friend auto operator- (const t& value, cx::matrix <t, n, ns...> m) noexcept {
		auto& elems = m.elems;
		ARITHMV(-= );

		return m;
	}

	[[nodiscard]] constexpr friend auto operator^ (cx::matrix <t, n, ns...> m, const t& value) noexcept {
		auto& elems = m.elems;
		ARITHMV(^= );

		return m;
	}

	[[nodiscard]] constexpr friend auto operator^ (const t& value, cx::matrix <t, n, ns...> m) noexcept {
		auto& elems = m.elems;
		ARITHMV(^= );

		return m;
	}

	[[nodiscard]] constexpr friend auto operator| (cx::matrix <t, n, ns...> m, const t& value) noexcept {
		auto& elems = m.elems;
		ARITHMV(|= );

		return m;
	}

	[[nodiscard]] constexpr friend auto operator| (const t& value, cx::matrix <t, n, ns...> m) noexcept {
		auto& elems = m.elems;
		ARITHMV(|= );

		return m;
	}

	[[nodiscard]] constexpr friend auto operator& (cx::matrix <t, n, ns...> m, const t& value) noexcept {
		auto& elems = m.elems;
		ARITHMV(&= );

		return m;
	}

	[[nodiscard]] constexpr friend auto operator& (const t& value, cx::matrix <t, n, ns...> m) noexcept {
		auto& elems = m.elems;
		ARITHMV(&= );

		return m;
	}

	[[nodiscard]] constexpr friend auto operator<< (cx::matrix <t, n, ns...> m, const t& value) noexcept {
		auto& elems = m.elems;
		ARITHMV(<<= );

		return m;
	}

	[[nodiscard]] constexpr friend auto operator>> (cx::matrix <t, n, ns...> m, const t& value) noexcept {
		auto& elems = m.elems;
		ARITHMV(>>= );

		return m;
	}
};
CX_END

#endif // MATRIX_HPP
