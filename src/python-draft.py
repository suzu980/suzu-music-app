import math

sample_rate =64 
N =16 #analyse 0.5 window

signal_length = 2048

xdata = []
for i in range(signal_length):
    t = i/sample_rate
    f1 = 4
    f2 =20
    xdata.append(
        math.cos(2 * math.pi * t * f1) +
        math.sin(2 * math.pi * t * f2) 
    )  # has presence on frequency 3 and 5

freq = []  

for i in range(N):
    freq.append(0.0)

def dft(x,f,k):
    hann = 0.5 - 0.5 * math.cos(2 * math.pi * k / N-1)
    # Apply Hann Window
    y = x[k]* hann
    # Perform DFT
    a = -2*math.pi*f*k/N
    result = complex(y,0) * (math.cos(a) + 1j*math.sin(a))
    return result

for f in range((int)(N/2)):
    x_re,x_im = 0,0
    for k in range(N):
        freq[f] += dft(xdata,f,k) 

## Nyquist theorem, Highest resolvable frequency is half of sample rate
for i in range((int)(N/2)):
    max_freq = sample_rate /2  
    freq_per_bin = (max_freq) / (N /2)
    q = i * freq_per_bin
    print("Frequency: %d\t%.2f" % (q,abs(freq[i]/N)))

## TODO Change scale to logarithmic scale

num_bins = (int)(N/2)
print("No of bins %s" % num_bins)
print("Log scaled bins")
logarithmic_bins = [10 ** (i / 10) for i in range(num_bins)]
## init bins
## twelve root of 2
twelve = 2 ** (1/12)
print(twelve)
powe = []
current = 0
elements = 0
while True:
    if current == 0:
        current = 1
        elements += 1
    else:
        current *= twelve
        elements += 1
    if current > 28: break
##print("Elements: %d" % elements)
