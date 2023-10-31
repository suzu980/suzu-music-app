import math

sample_rate =16 
N = 16#analyse 0.5 window

signal_length =16 

xdata = []
for i in range(signal_length):
    t = i/sample_rate
    f1 = 4
    f2 =0
    xdata.append(
        math.cos(2 * math.pi * t * f1) +
        math.sin(2 * math.pi * t * f2) 
    )  # has presence on frequency 3 and 5


## TODO FFT
def fft(x,n, max_N):
    if n == max_N: 
        for k in range(n):
            hann = 0.5 - 0.5 * math.cos(2 * math.pi * k / N-1)
            # Apply Hann Window
            x[k] = x[k]* hann
    if n <= 1: return x
    even = fft(x[0::2],n/2, N)
    odd = fft(x[1::2],n/2, N)
    # Perform fft 
    T = []
    for k in range(int(n/2)):
        a = -2*math.pi*k/n
        T = complex(odd[k],0) * (math.cos(a) + 1j*math.sin(a))
        x[k] = even[k] + T 
        x[(int)(n/2) + k] = even[k] - T
    return x  

def dft(x,n):
    fq = []
    for f in range((int)(n/2)):
        new_dat = 0
        for k in range((int)(n)):
            hann = 0.5 - 0.5 * math.cos(2 * math.pi * k / N-1)
            # Apply Hann Window
            y = x[k] * hann
            # Perform DFT
            a = -2*math.pi*f*k/N
            result = complex(y,0) * (math.cos(a) + 1j*math.sin(a))
            new_dat += result
        fq.append(new_dat)
    return fq 

dft_freq = []
fft_freq = []
dft_freq  = dft(xdata, N)
fft_freq  = fft(xdata, N, N)


## Nyquist theorem, Highest resolvable frequency is half of sample rate
for i in range((int)(N/2)):
    max_freq = sample_rate /2  
    freq_per_bin = (max_freq) / (N /2)
    q = i * freq_per_bin
    power_spec = abs(dft_freq[i]) 
    r = power_spec 
    log_spec = math.log(r)/1
    print("DFT Frequency: %d\t%.2f" % (q, power_spec))
print()
for i in range((int)(N/2)):
    max_freq = sample_rate /2  
    freq_per_bin = (max_freq) / (N /2)
    q = i * freq_per_bin
    power_spec = abs(fft_freq[i]) 
    r = power_spec 
    log_spec = math.log(r)/1
    print("FFT Frequency: %d\t%.2f" % (q, power_spec))
