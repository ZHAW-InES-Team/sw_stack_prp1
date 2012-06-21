# /********************************************************************
# *
# *  Copyright (c) 2008, Institute of Embedded Systems at 
# *                      Zurich University of Applied Sciences 
# *                      (http://ines.zhaw.ch)
# *
# *  All rights reserved.
# *
# *
# *  Redistribution and use in source and binary forms, with or  
# *  without modification, are permitted provided that the 
# *  following conditions are met:
# *
# *  - Redistributions of source code must retain the above copyright 
# *    notice, this list of conditions and the following disclaimer. 
# *
# *  - Redistributions in binary form must reproduce the above 
# *    copyright notice, this list of conditions and the following 
# *    disclaimer in the documentation and/or other materials provided
# *    with the distribution. 
# *
# *  - Neither the name of the Zurich University of Applied Sciences
# *    nor the names of its contributors may be used to endorse or 
# *    promote products derived from this software without specific 
# *    prior written permission. 
# *
# *
# *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
# *  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
# *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
# *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
# *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS 
# *  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
# *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
# *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
# *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
# *  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
# *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
# *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
# *  POSSIBILITY OF SUCH DAMAGE.
# *
# *********************************************************************/

# Edit this for verbosity levels of debug output (5: very verbose)
# For using debug prints, uncomment the DEBUG flag
PRP_DEBUG_LEVEL		= 1
PRP_DISCARD_DEBUG_LEVEL	= 1
# DEBUG   = -DPRP_DEBUG_LOG -DPRP_DEBUG_LEVEL=$(PRP_DEBUG_LEVEL) -DPRP_DISCARD_DEBUG_LEVEL=$(PRP_DISCARD_DEBUG_LEVEL)
CFLAGS	=  -Wall -g2 $(DEBUG) -Werror
CFLAGS	+= $(DEBFLAGS) $(CFLAGS_EXTRA)
LDFLAGS = -lm -lpcap $(LDFLAGS_EXTRA)

TARGET 	= prp_pcap_tap_userspace

all: clean $(TARGET)

OBJ = PRP_NetItf_T_linux.o
OBJ += PRP_OsItf_T_linux.o
OBJ += ../prp/PRP_DiscardAlgorithm_T.o
OBJ += ../prp/PRP_Environment_T.o
OBJ += ../prp/PRP_EnvironmentConfiguration_T.o
OBJ += ../prp/PRP_Frames_T.o
OBJ += ../prp/PRP_FrameAnalyser_T.o
OBJ += ../prp/PRP_LogItf_T.o
OBJ += ../prp/PRP_RedundancyControlTrailer_T.o
OBJ += ../prp/PRP_Supervision_T.o
OBJ += ../prp/PRP_T.o
OBJ += ../prp/PRP_Timer_T.o
OBJ += ../prp/PRP_Trailer_T.o
OBJ += ../prp/PRP_Lock_T.o

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:	
	rm -f *.o ../prp/*.o
	rm -f $(TARGET)
