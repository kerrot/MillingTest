#pragma once

class Material
{
public:
	Material(unsigned int a_width, unsigned int a_length, float a_height);
	~Material();

private:
	float* m_data;
};