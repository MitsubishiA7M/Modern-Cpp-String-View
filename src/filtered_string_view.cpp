#include "./filtered_string_view.h"
#include <algorithm>
#include <cstring>
#include <iterator>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace fsv {

    // Free default predicate
    filter filtered_string_view::default_predicate = [](const char&) { return true; };

    // Constructors
    filtered_string_view::filtered_string_view() noexcept
    : ptr_(nullptr)
    , len_(0)
    , pred_(default_predicate) {}

    filtered_string_view::filtered_string_view(const std::string& str, filter pred)
    : ptr_(str.data())
    , len_(0)
    , pred_(std::move(pred)) {
        len_ = count_filtered();
    }

    filtered_string_view::filtered_string_view(const char* s, filter pred)
    : ptr_(s)
    , len_(0)
    , pred_(std::move(pred)) {
        len_ = count_filtered();
    }

    // Move semantics
    filtered_string_view::filtered_string_view(filtered_string_view&& other) noexcept
    : ptr_(other.ptr_)
    , len_(other.len_)
    , pred_(std::move(other.pred_)) {
        other.ptr_ = nullptr;
        other.len_ = 0;
        other.pred_ = std::function<bool(const char&)>{};
    }

    filtered_string_view& filtered_string_view::operator=(filtered_string_view&& other) noexcept {
        if (this != &other) {
            ptr_ = other.ptr_;
            len_ = other.len_;
            pred_ = std::move(other.pred_);
            other.ptr_ = nullptr;
            other.len_ = 0;
            other.pred_ = std::function<bool(const char&)>{};
        }
        return *this;
    }

    // Capacity
    std::size_t filtered_string_view::size() const noexcept {
        return len_;
    }
    bool filtered_string_view::empty() const noexcept {
        return len_ == 0;
    }

    // Element access
    const char* filtered_string_view::data() const noexcept {
        return ptr_;
    }
    const filter& filtered_string_view::predicate() const noexcept {
        return pred_;
    }
    const char& filtered_string_view::operator[](std::size_t idx) const {
        return ptr_[filtered_index_to_raw(idx)];
    }
    const char& filtered_string_view::at(std::size_t idx) const {
        if (idx >= len_)
            throw std::domain_error("filtered_string_view::at(" + std::to_string(idx) + "): invalid index");
        return (*this)[idx];
    }

    // Conversion to std::string
    filtered_string_view::operator std::string() const {
        std::string result;
        result.reserve(len_);
        if (!ptr_)
            return result;
        for (const char* p = ptr_; p && *p; ++p) {
            if (pred_(*p))
                result.push_back(*p);
        }
        return result;
    }

    // Helpers
    std::size_t filtered_string_view::count_filtered() const {
        if (!ptr_)
            return 0;
        std::size_t cnt = 0;
        for (const char* p = ptr_; *p; ++p)
            if (pred_(*p))
                ++cnt;
        return cnt;
    }

    std::size_t filtered_string_view::filtered_index_to_raw(std::size_t idx) const {
        if (!ptr_)
            throw std::out_of_range("filtered_string_view: index out of range");
        std::size_t seen = 0;
        for (std::size_t i = 0; ptr_[i] != '\0'; ++i) {
            if (pred_(ptr_[i])) {
                if (seen++ == idx)
                    return i;
            }
        }
        throw std::out_of_range("filtered_string_view: index out of range");
    }

    // const_iterator implementations
    filtered_string_view::const_iterator::const_iterator() noexcept
    : fsv_(nullptr)
    , raw_pos_(0) {}

    filtered_string_view::const_iterator::const_iterator(const filtered_string_view* fsv, std::size_t raw_pos) noexcept
    : fsv_(fsv)
    , raw_pos_(raw_pos) {
        if (!fsv_ || !fsv_->ptr_)
            return;

        std::size_t len = std::strlen(fsv_->ptr_);
        while (raw_pos_ < len && !fsv_->pred_(fsv_->ptr_[raw_pos_])) {
            ++raw_pos_;
        }
    }

    filtered_string_view::const_iterator::reference filtered_string_view::const_iterator::operator*() const noexcept {
        return fsv_->ptr_[raw_pos_];
    }

    filtered_string_view::const_iterator& filtered_string_view::const_iterator::operator++() {
        if (!fsv_ || !fsv_->ptr_)
            return *this;
        std::size_t len = std::strlen(fsv_->ptr_);
        do {
            ++raw_pos_;
        } while (raw_pos_ < len && !fsv_->pred_(fsv_->ptr_[raw_pos_]));
        return *this;
    }

    filtered_string_view::const_iterator filtered_string_view::const_iterator::operator++(int) {
        const_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    filtered_string_view::const_iterator& filtered_string_view::const_iterator::operator--() {
        if (!fsv_ || !fsv_->ptr_)
            return *this;
        do {
            --raw_pos_;
        } while (raw_pos_ > 0 && !fsv_->pred_(fsv_->ptr_[raw_pos_]));
        return *this;
    }

    filtered_string_view::const_iterator filtered_string_view::const_iterator::operator--(int) {
        const_iterator tmp = *this;
        --*this;
        return tmp;
    }

    bool filtered_string_view::const_iterator::operator==(const const_iterator& o) const noexcept {
        return fsv_ == o.fsv_ && raw_pos_ == o.raw_pos_;
    }

    bool filtered_string_view::const_iterator::operator!=(const const_iterator& o) const noexcept {
        return !(*this == o);
    }

    // Iterator range functions
    filtered_string_view::iterator filtered_string_view::begin() const {
        return const_iterator(this, 0);
    }
    filtered_string_view::iterator filtered_string_view::end() const {
        return const_iterator(this, ptr_ ? std::strlen(ptr_) : 0);
    }
    filtered_string_view::const_iterator filtered_string_view::cbegin() const {
        return begin();
    }
    filtered_string_view::const_iterator filtered_string_view::cend() const {
        return end();
    }
    filtered_string_view::reverse_iterator filtered_string_view::rbegin() const {
        return reverse_iterator(end());
    }
    filtered_string_view::reverse_iterator filtered_string_view::rend() const {
        return reverse_iterator(begin());
    }
    filtered_string_view::const_reverse_iterator filtered_string_view::crbegin() const {
        return const_reverse_iterator(end());
    }
    filtered_string_view::const_reverse_iterator filtered_string_view::crend() const {
        return const_reverse_iterator(begin());
    }

    // Non-member utility: compose multiple filters
    filtered_string_view compose(const filtered_string_view& fsv_obj, const std::vector<filter>& filts) {
        auto pred = [filts](const char& c) {
            for (const auto& f : filts)
                if (!f(c))
                    return false;
            return true;
        };
        return filtered_string_view(fsv_obj.data(), std::move(pred));
    }

    // Non‑member split implementation (part 1)
    std::vector<filtered_string_view> split(const filtered_string_view& fsv, const filtered_string_view& tok) {
        std::vector<filtered_string_view> result;
        std::string s = static_cast<std::string>(fsv);
        std::string d = static_cast<std::string>(tok);

        if (s.empty() || d.empty()) {
            result.push_back(fsv);
            return result;
        }

        size_t start = 0;
        size_t pos = s.find(d);

        while (pos != std::string::npos) {
            result.push_back(substr(fsv, start, pos - start));
            start = pos + d.length();
            pos = s.find(d, start);
        }

        result.push_back(substr(fsv, start));
        return result;
    }

    // Non‑member substr implementation
    filtered_string_view substr(const filtered_string_view& fsv, std::size_t pos, std::optional<std::size_t> count) {
        std::size_t sz = fsv.size();
        if (pos > sz) {
            throw std::out_of_range("filtered_string_view::substr(" + std::to_string(pos)
                                    + "): position out of range for filtered string of size " + std::to_string(sz));
        }

        if (pos == sz || (count && *count == 0)) {
            const char* base = fsv.data();
            auto empty_pred = [](const char&) { return false; };
            return filtered_string_view(base, std::move(empty_pred));
        }

        std::size_t end = count ? std::min(pos + *count, sz) : sz;
        std::string filtered = static_cast<std::string>(fsv);
        const char* base = fsv.data();
        filter orig = fsv.predicate();

        auto new_pred = [base, orig, filtered, pos, end](const char& c) -> bool {
            if (!orig(c))
                return false;
            std::size_t filtered_idx = 0;
            const char* p = base;
            while (p <= &c && *p) {
                if (p == &c) {
                    return filtered_idx >= pos && filtered_idx < end;
                }
                if (orig(*p)) {
                    filtered_idx++;
                }
                p++;
            }
            return false;
        };

        return filtered_string_view(base, std::move(new_pred));
    }

    // Non-member comparisons
    bool operator==(const filtered_string_view& a, const filtered_string_view& b) {
        return static_cast<std::string>(a) == static_cast<std::string>(b);
    }
    bool operator!=(const filtered_string_view& a, const filtered_string_view& b) {
        return !(a == b);
    }
    bool operator<(const filtered_string_view& a, const filtered_string_view& b) {
        return static_cast<std::string>(a) < static_cast<std::string>(b);
    }
    bool operator<=(const filtered_string_view& a, const filtered_string_view& b) {
        return !(b < a);
    }
    bool operator>(const filtered_string_view& a, const filtered_string_view& b) {
        return b < a;
    }
    bool operator>=(const filtered_string_view& a, const filtered_string_view& b) {
        return !(a < b);
    }

    // Output operator
    std::ostream& operator<<(std::ostream& os, const filtered_string_view& fsv) {
        for (auto c : fsv)
            os << c;
        return os;
    }

} // namespace fsv