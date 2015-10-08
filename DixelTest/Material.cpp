#include "Material.h"

Material::Material( unsigned int a_width, unsigned int a_length, float a_height )
:
m_data(0)
{
	if (a_width > 0 && a_height > 0 && a_length > 0)
	{
		m_data = new float[a_width * a_length];
		for (int i = 0; i < a_width * a_length; ++i)
		{
			m_data[i] = a_height;
		}
	}
}

Material::~Material()
{
	if (m_data)
	{
		delete[] m_data;
		m_data = 0;
	}
}

