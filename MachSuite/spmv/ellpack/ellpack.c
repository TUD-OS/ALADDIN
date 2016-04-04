/*
Copyright (c) 2014, the President and Fellows of Harvard College.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of Harvard University nor the names of its contributors may
  be used to endorse or promote products derived from this software without
  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Based on algorithm described here:
http://www.cs.berkeley.edu/~mhoemmen/matrix-seminar/slides/UCB_sparse_tutorial_1.pdf
*/
#include "ellpack.h"
void ellpack(TYPE nzval[N*L], int cols[N*L], TYPE vec[N], TYPE out[N])
{
#ifdef DMA_MODE
  dmaLoad(&nzval[0],0*512*8,512*8*8);
  dmaLoad(&nzval[0],1*512*8,512*8*8);
  dmaLoad(&nzval[0],2*512*8,512*8*8);
  dmaLoad(&nzval[0],3*512*8,512*8*8);
  dmaLoad(&nzval[0],4*512*8,512*8*8);
  dmaLoad(&nzval[0],5*512*8,512*8*8);
  dmaLoad(&nzval[0],6*512*8,512*8*8);
  dmaLoad(&nzval[0],7*512*8,512*8*8);
  dmaLoad(&nzval[0],8*512*8,512*8*8);
  dmaLoad(&nzval[0],9*512*8,332*8*8);
  dmaLoad(&cols[0],0*1024*4,1024*4*8);
  dmaLoad(&cols[0],1*1024*4,1024*4*8);
  dmaLoad(&cols[0],2*1024*4,1024*4*8);
  dmaLoad(&cols[0],3*1024*4,1024*4*8);
  dmaLoad(&cols[0],4*1024*4,844*4*8);
  dmaLoad(&vec[0],0,494*8*8);
  dmaLoad(&out[0],0,494*8*8);
#endif
    int i, j;
    TYPE Si;

    ellpack_1 : for (i=0; i<N; i++) {
        TYPE sum = out[i];
        ellpack_2 : for (j=0; j<L; j++) {
                Si = nzval[j + i*L] * vec[cols[j + i*L]];
                sum += Si;
        }
        out[i] = sum;
    }
#ifdef DMA_MODE
  dmaStore(&out[0],0,494*8*8);
#endif
}
