/**
 * @description Represents a uniform shader attribute
 * TODO: support for matrices
 * 
 * @file ShaderUniformValue.h
 * @author Jo�o Pedro Jorge
 */
#pragma once

#include "ShaderUniformObject.h"

template <class T>
class ShaderUniformValue: public ShaderUniformObject
{
public:
	ShaderUniformValue(void);
	virtual ~ShaderUniformValue(void);

	void setValue(T value);

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
void ShaderUniformValue<T>::use()
{
	if ( this->getHasChanged() )
	{
		setUniform(this->location, this->value);
	}
}
