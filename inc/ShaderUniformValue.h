/**
 * @description Represents a uniform shader attribute
 * TODO: support for matrices
 * 
 * @file ShaderUniformValue.h
 * @author João Pedro Jorge
 */
#pragma once

#include "ShaderUniformObject.h"

template <class T>
class ShaderUniformValue: public ShaderUniformObject
{
public:
	ShaderUniformValue(void);
	ShaderUniformValue(char* name, T value);
	virtual ~ShaderUniformValue(void);

	void setValue(T value);

	void set(char* name, T value);

	T getValue() const;
	
	virtual void use();

	// attributes
private:
	T value;
};

template <class T>
ShaderUniformValue<T>::ShaderUniformValue(void)
{
}

template <class T>
ShaderUniformValue<T>::ShaderUniformValue(char* name, T value)
{
	this->set(name, value);
}

template <class T>
ShaderUniformValue<T>::~ShaderUniformValue(void)
{
}

/**
 * @param value the attribute value
 */
template <class T>
void ShaderUniformValue<T>::setValue(T value)
{
	this->value = value;

	// NOTE: we must call the upper class and MARK the
	// value as CHANGED
	this->setHasChanged( true );
}


template <class T>
T ShaderUniformValue<T>::getValue() const
{
	return this->value;
}

template <class T>
void ShaderUniformValue<T>::set(char* name, T value)
{
	this->setName(name);
	this->setValue(value);
}

template <class T>
void ShaderUniformValue<T>::use()
{
	if ( this->getHasChanged() )
	{
		setUniform(this->location, this->value);
	}
}

