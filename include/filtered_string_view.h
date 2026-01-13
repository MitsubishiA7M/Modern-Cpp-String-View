#ifndef COMP6771_ASS2_FSV_H
#define COMP6771_ASS2_FSV_H

#include <compare>
#include <functional>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

namespace fsv {
    using filter = std::function<bool(const char&)>;

    class filtered_string_view {
    public:
        // Static member
        static filter default_predicate;

        // Iterator declaration and aliases
        class const_iterator {
        public:
            using value_type = char;
            using reference = const char&;
            using pointer = void;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::bidirectional_iterator_tag;

            const_iterator() noexcept;
            const_iterator(const filtered_string_view* fsv, std::size_t raw_pos) noexcept;

            reference operator*() const noexcept;
            const_iterator& operator++();
            const_iterator operator++(int);
            const_iterator& operator--();
            const_iterator operator--(int);
            bool operator==(const const_iterator& o) const noexcept;
            bool operator!=(const const_iterator& o) const noexcept;

        private:
            const filtered_string_view* fsv_ = nullptr;
            std::size_t raw_pos_;
        };

        using iterator = const_iterator;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using reverse_iterator = const_reverse_iterator;

        // Constructors
        filtered_string_view() noexcept;
        filtered_string_view(const std::string& str, filter pred = filtered_string_view::default_predicate);
        filtered_string_view(const char* s, filter pred = filtered_string_view::default_predicate);
        filtered_string_view(const filtered_string_view& other) = default;
        filtered_string_view(filtered_string_view&& other) noexcept;
        ~filtered_string_view() = default;

        // Assignment
        filtered_string_view& operator=(const filtered_string_view& other) = default;
        filtered_string_view& operator=(filtered_string_view&& other) noexcept;

        // Capacity
        std::size_t size() const noexcept;
        bool empty() const noexcept;

        // Access
        const char* data() const noexcept;
        const filter& predicate() const noexcept;
        const char& operator[](std::size_t idx) const;
        const char& at(std::size_t idx) const;
        explicit operator std::string() const;

        // Iterators
        iterator begin() const;
        iterator end() const;
        const_iterator cbegin() const;
        const_iterator cend() const;
        reverse_iterator rbegin() const;
        reverse_iterator rend() const;
        const_reverse_iterator crbegin() const;
        const_reverse_iterator crend() const;

        // Comparison operators
        friend bool operator==(const filtered_string_view& lhs, const filtered_string_view& rhs);
        friend bool operator!=(const filtered_string_view& lhs, const filtered_string_view& rhs);
        friend bool operator<(const filtered_string_view& lhs, const filtered_string_view& rhs);
        friend bool operator<=(const filtered_string_view& lhs, const filtered_string_view& rhs);
        friend bool operator>(const filtered_string_view& lhs, const filtered_string_view& rhs);
        friend bool operator>=(const filtered_string_view& lhs, const filtered_string_view& rhs);

        // Output
        friend std::ostream& operator<<(std::ostream& os, const filtered_string_view& fsv);

        // Allow free-function substr to access private helpers
        friend filtered_string_view
        substr(const filtered_string_view& fsv, std::size_t pos, std::optional<std::size_t> count);

    private:
        const char* ptr_ = nullptr;
        std::size_t len_ = 0;
        filter pred_ = default_predicate;

        std::size_t count_filtered() const;
        std::size_t filtered_index_to_raw(std::size_t idx) const;
    };

    // Non-member utilities
    filtered_string_view compose(const filtered_string_view& fsv, const std::vector<filter>& filts);
    std::vector<filtered_string_view> split(const filtered_string_view& fsv, const filtered_string_view& tok);
    filtered_string_view
    substr(const filtered_string_view& fsv, std::size_t pos = 0, std::optional<std::size_t> count = std::nullopt);

} // namespace fsv

#endif // COMP6771_ASS2_FSV_H