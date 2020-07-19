import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator

plt.rcParams['text.usetex'] = True #Let TeX do the typsetting
plt.rcParams['text.latex.preamble'] = [r'\usepackage{sansmath}', r'\sansmath'] #Force sans-serif math mode (for axes labels)
plt.rcParams['font.family'] = 'sans-serif' # ... for regular text
plt.rcParams['font.sans-serif'] = 'Helvetica' # Choose a nice font here

def f(s): return [float(x) for x in s.split()]

def main():
    plt.style.use('default')
    plt.figure(figsize=(15,6))

    labels = ['RandomContraction-LINE', 'SchurComplement-LINE', 'OriginalGraph-LINE']
    markers = ['o', '^', 's']
    algorithms = labels

    """
    0 = GRC
    1 = SC
    2 = benchmark
    """

    # --------------------------------------------------------------------------
    # Blog catalog micro NetMF
    plt.subplot(2, 3, 1)
    x = f("10.00	20.00	30.00	40.00	50.00	60.00	70.00	80.00	90.00")
    y = [[] for i in range(6)]
    y[0] = f("32.74	36.15	37.73	38.54	39	39.4	39.73	40.58	41.037")
    y[1] = f("33.51	36.74	38.23	39.04	39.88	40.33	40.34	40.95	40.89")
    y[2] = f("32.45	35.75	37.26	38.05	38.86	39.28	39.55	39.82	39.67")
    for i in range(3):
        plt.plot(x, y[i], label=labels[i], marker=markers[i])
    plt.grid()
    plt.title('BlogCatalog')
    plt.ylabel('Micro-F1 (%)', fontsize='large')

    # Blog catalog macro NetMF
    plt.subplot(2, 3, 4)
    y = [[] for i in range(3)]
    y[0] = f("13.71	17.07	18.71	19.73	20.27	21	21.16	21.73	22.04")
    y[1] = f("14.5	17.9	19.46	20.57	21.33	21.92	21.92	22.23	21.72")
    y[2] = f("12.95	16.03	17.46	18.33	19.14	19.59	19.84	19.96	19.41")
    for i in range(3):
        plt.plot(x, y[i], label=labels[i], marker=markers[i])
    plt.grid()
    plt.ylabel('Macro-F1 (%)', fontsize='large')

    # --------------------------------------------------------------------------
    # Flickr micro
    plt.subplot(2, 3, 2)
    x = f("1.00	2.00	3.00	4.00	5.00	6.00	7.00	8.00	9.00	10.00")
    y = [[] for i in range(3)]
    y[0] = f("21.82	23.82	25.23	26.12	26.8	27.41	27.86	28.25	28.56	28.83")
    y[1] = f("22.08	24.2	25.58	26.52	27.29	27.88	28.34	28.77	29.12	29.41")
    y[2] = f("21.8	23.86	25.2	26.1	26.86	27.43	27.91	28.34	28.68	28.98")
    for i in range(3):
        plt.plot(x, y[i], label=labels[i], marker=markers[i])
    plt.grid() 
    plt.title('Flickr')

    # Flickr macro
    plt.subplot(2, 3, 5)
    y = [[] for i in range(3)]
    y[0] = f("4.65	6.3	7.69	8.6	9.2	9.9	10.35	10.79	11.18	11.58")
    y[1] = f("5.06	6.83	8.04	9.02	9.8	10.51	10.99	11.54	11.99	12.29")
    y[2] = f("4.8	6.51	7.62	8.51	9.23	9.85	10.31	10.84	11.24	11.54")
    for i in range(3):
        plt.plot(x, y[i], label=labels[i], marker=markers[i])
    plt.grid()

    # --------------------------------------------------------------------------
    # YouTube micro
    plt.subplot(2, 3, 3)
    x = f("1.00	2.00	3.00	4.00	5.00	6.00	7.00	8.00	9.00	10.00")
    y = [[] for i in range(3)]
    y[0] = f("27.63	30.5	32.44	33.5	34.32	34.92	35.39	35.73	36.1	36.34")
    y[1] = f("26.73	28.33	29.43	30.22	30.77	31.28	31.69	31.97	32.27	32.53")
    y[2] = f("26.22	27.64	28.52	29.24	29.75	30.18	30.53	30.8	31.07	31.3")
    for i in range(3):
        plt.plot(x, y[i], label=labels[i], marker=markers[i])
    plt.grid() 
    plt.title('YouTube')

    # YouTube macro
    plt.subplot(2, 3, 6)
    y = [[] for i in range(3)]
    y[0] = f("10.86	15.22	18.29	20.01	21.32	22.31	23.11	23.7	24.32	24.78")
    y[1] = f("9.62	12.16	14.12	15.47	16.42	17.29	18	18.46	19	19.46")
    y[2] = f("8.7	10.88	12.49	13.7	14.58	15.3	15.86	16.32	16.78	17.14")
    for i in range(3):
        plt.plot(x, y[i], label=labels[i], marker=markers[i])

    # Output ----------------------------
    plt.grid()
    plt.legend(labels=algorithms, loc='lower center',
               bbox_to_anchor=(-0.70, -0.3), fancybox=False, shadow=False, ncol=3)

    
    plt.savefig("f1-line-embeddings.png", transparent=True, bbox_inches='tight', dpi=256)
    plt.show()

main()
