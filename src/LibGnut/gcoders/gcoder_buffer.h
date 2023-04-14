#ifndef GCODER_BUFFER_H
#define GCODER_BUFFER_H

#include <string>
#include <deque>
#include <queue>

using namespace std;


namespace gnut {
	class gcoder_buffer 
	{
	public:
		/** @brief constructor. */
		gcoder_buffer();
		/** @brief destructor. */
		~gcoder_buffer();
		/** @brief number of char elements in buffer (excl \0). */
		int size();
		/** @brief cummulate buffer. */
		int add(char* buff, int size);
		/** @brief get single line from the buffer. */
		int getline(string& str, int from_pos);
		/** @brief remove from buffer. */
		int consume(int bytes_to_eat);
		
	private:

		deque<char> _buffer;  ///< buffer vector
	};

	class gcoder_char_buffer
	{
	public:
		/** @brief constructor. */
		gcoder_char_buffer();
		/** @brief destructor. */
		~gcoder_char_buffer();
		/** @brief number of char elements in buffer (excl \0). */
		int size();
		/** @brief cummulate buffer. */
		int add(char* buff, int size);
		/** @brief get single line from the buffer. */
		int getline(string& str, int from_pos);
		/** @brief remove from buffer. */
		int consume(int bytes_to_eat);

	private:

		int _begpos;     ///< begin position of useful data
		int _endpos;     ///< after last position
		int _buffsz;     ///< size of buffer
		char* _buffer;   ///< class buffer
	};
}

#endif