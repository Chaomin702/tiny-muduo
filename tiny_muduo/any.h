#pragma once
#include <typeinfo>
#include <memory>
#include <exception>
namespace cm {
		class Any {
	public:
		template<typename ValueType> friend const ValueType* anyCast(const Any* any);
		class HolderBase {
		public:
			virtual ~HolderBase() {}
			virtual HolderBase* clone() = 0;
			virtual const std::type_info& type() = 0;
		};
		template<typename ValueType>
		class Holder : public HolderBase {
		public:
			Holder():value_(0){}
			Holder(const ValueType& value):value_(value){}
			Holder* clone() { return new Holder(value_); }
			const std::type_info& type() { return typeid(value_); }
			ValueType value_;
		};
		Any():value_(nullptr){}
		template<typename ValueType>
		Any(const ValueType& value):value_(new Holder<ValueType>(value)){}
		Any(const Any& rhs):value_(rhs.value_->clone()){}
		Any(Any&& rhs) :value_(rhs.value_) {
			rhs.value_ = nullptr;
		}
		Any& operator=(Any rhs) noexcept {
			swap(rhs);
			return *this;
		}
		Any& operator=(Any &&rhs) noexcept {
			swap(rhs);
			return *this;
		}
		Any& swap(Any& rhs) noexcept{
			std::swap(value_, rhs.value_);
			return *this;
		}
		~Any() { delete value_; }
		bool empty()const { return !value_; }
		void clear() { Any().swap(*this); }
		const std::type_info& type()const { return value_ ? value_->type() : typeid(void); }
	private:
		HolderBase *value_;
	};
	template<typename ValueType>
	const ValueType* anyCast(const Any* any) {
		return (any && any->type() == typeid(ValueType)) ?
			&static_cast<Any::Holder<ValueType>*>(any->value_)->value_ : 0;
	}
	template<typename ValueType>
	ValueType* anyCast(Any* any) {
		return const_cast<ValueType*>(anyCast<ValueType>(static_cast<const Any*>(any)));
	}
	template<typename ValueType>
	const ValueType anyCast(const Any& any) {
		auto *t = anyCast<ValueType>(&any);
		if (!t)
			throw std::runtime_error("Any type incorrect!");
		return *t;
	}
	template<typename ValueType>
	ValueType anyCast(Any& any) {
		return anyCast<ValueType>(const_cast<const Any&>(any));
	}
}