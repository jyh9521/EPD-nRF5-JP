CC = gcc
CFLAGS = -Wall -IGUI -D__HEAP_SIZE=32768
LDFLAGS = -lgdi32 -mwindows

SRCS = GUI/Adafruit_GFX.c GUI/u8g2_font.c GUI/fonts.c GUI/GUI.c GUI/JapaneseCalendar.c GUI/JapaneseRokuyo.c GUI/Lunar.c emulator.c
OBJS = $(SRCS:.c=.o)
TARGET = emulator.exe
TEST_TARGET = tests/test_japanese_calendar.exe
TEST_ROKUYO_TARGET = tests/test_japanese_rokuyo.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(TEST_TARGET) $(TEST_ROKUYO_TARGET)

test: $(TEST_TARGET) $(TEST_ROKUYO_TARGET)
	./$(TEST_TARGET)
	./$(TEST_ROKUYO_TARGET)

$(TEST_TARGET): tests/test_japanese_calendar.c GUI/JapaneseCalendar.c GUI/JapaneseCalendar.h
	$(CC) $(CFLAGS) tests/test_japanese_calendar.c GUI/JapaneseCalendar.c -o $@

$(TEST_ROKUYO_TARGET): tests/test_japanese_rokuyo.c GUI/JapaneseRokuyo.c GUI/JapaneseRokuyo.h GUI/JapaneseCalendar.c GUI/Lunar.c
	$(CC) $(CFLAGS) tests/test_japanese_rokuyo.c GUI/JapaneseRokuyo.c GUI/JapaneseCalendar.c GUI/Lunar.c -o $@
