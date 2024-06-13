/***************************************************************************
                          trace.h  -  To create an XML trace file
                             -------------------
    begin                : Mon Dec 17 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __TRACE_H__
#define __TRACE_H__

#include <fstream>

using namespace std;

class TraceFile
{
	public:
		TraceFile(const char *filename)
		{
			file = new ofstream(filename);
			(*file) << "<trace>" << endl;
		}
		~TraceFile()
		{
			(*file) << "</trace>" << endl;
			delete file;
		}
		
		void Begin(const char *tagName) { (*file) << "<" << tagName << ">" << endl; }
		void End(const char *tagName) { (* file) << "</" << tagName << ">" << endl; }
		
		friend TraceFile& operator << (TraceFile& tf, int v);
		friend TraceFile& operator << (TraceFile& tf, unsigned int v);
		friend TraceFile& operator << (TraceFile& tf, char v);
		friend TraceFile& operator << (TraceFile& tf, unsigned char v);
		friend TraceFile& operator << (TraceFile& tf, long v);
		friend TraceFile& operator << (TraceFile& tf, unsigned long v);
		friend TraceFile& operator << (TraceFile& tf, long long v);
		friend TraceFile& operator << (TraceFile& tf, unsigned long long v);
		friend TraceFile& operator << (TraceFile& tf, float v);
		friend TraceFile& operator << (TraceFile& tf, double v);
		friend TraceFile& operator << (TraceFile& tf, const char *s);

		void Value(const char *name, int value)
		{
			(*file) << "<" << name << ">" << value << "</" << name << ">" << endl;
		}
		void Value(const char *name, unsigned int value)
		{
			(*file) << "<" << name << ">" << value << "</" << name << ">" << endl;
		}
		void Value(const char *name, unsigned char value)
		{
			(*file) << "<" << name << ">" << (unsigned int) value << "</" << name << ">" << endl;
		}
		void Value(const char *name, char value)
		{
			(*file) << "<" << name << ">" << (int) value << "</" << name << ">" << endl;
		}
		void Value(const char *name, unsigned long value)
		{
			(*file) << "<" << name << ">" << value << "</" << name << ">" << endl;
		}
		void Value(const char *name, long value)
		{
			(*file) << "<" << name << ">" << value << "</" << name << ">" << endl;
		}
		void Value(const char *name, unsigned long long value)
		{
			(*file) << "<" << name << ">" << value << "</" << name << ">" << endl;
		}
		void Value(const char *name, long long value)
		{
			(*file) << "<" << name << ">" << value << "</" << name << ">" << endl;
		}
		void Value(const char *name, float value)
		{
			(*file) << "<" << name << ">" << value << "</" << name << ">" << endl;
		}
		void Value(const char *name, double value)
		{
			(*file) << "<" << name << ">" << value << "</" << name << ">" << endl;
		}
		void Value(const char *name, const char *s)
		{
			(*file) << "<" << name << ">" << s << "</" << name << ">" << endl;
		}
		void Value(const char *name, bool value)
		{
			(*file) << "<" << name << ">" << (value ? "true" : "false") << "</" << name << ">" << endl;
		}
	
	private:
		ofstream *file;
};

inline TraceFile& operator << (TraceFile& tf, int v)
{
	(*tf.file) << v;
}

inline TraceFile& operator << (TraceFile& tf, unsigned int v)
{
	(*tf.file) << v;
}

inline TraceFile& operator << (TraceFile& tf, char v)
{
	(*tf.file) << v;
}

inline TraceFile& operator << (TraceFile& tf, unsigned char v)
{
	(*tf.file) << v;
}

inline TraceFile& operator << (TraceFile& tf, long v)
{
	(*tf.file) << v;
}

inline TraceFile& operator << (TraceFile& tf, unsigned long v)
{
	(*tf.file) << v;
}

inline TraceFile& operator << (TraceFile& tf, long long v)
{
	(*tf.file) << v;
}

inline TraceFile& operator << (TraceFile& tf, unsigned long long v)
{
	(*tf.file) << v;
}

inline TraceFile& operator << (TraceFile& tf, float v)
{
	(*tf.file) << v;
}

inline TraceFile& operator << (TraceFile& tf, double v)
{
	(*tf.file) << v;
}

inline TraceFile& operator << (TraceFile& tf, const char *s)
{
	(*tf.file) << s;
}



extern TraceFile *trace_file;

#endif
