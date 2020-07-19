import matplotlib.pyplot as plt

plt.rcParams['text.usetex'] = True #Let TeX do the typsetting
plt.rcParams['text.latex.preamble'] = [r'\usepackage{sansmath}', r'\sansmath'] #Force sans-serif math mode (for axes labels)
plt.rcParams['font.family'] = 'sans-serif' # ... for regular text
plt.rcParams['font.sans-serif'] = 'Helvetica' # Choose a nice font here

def f(s): return [float(x) for x in s.split()]

def main():
    plt.style.use('default')
    plt.figure(figsize=(15,6))

    labels = ['RandomContraction-NetMF', 'SchurComplement-NetMF', 'OriginalGraph-NetMF', 'OriginalGraph-NetSMF']
    markers = ['o', '^', 's']
    markers = ['o', '^', 's', 'd']
    algorithms = labels
    n = len(labels)
    
    """
    0 = GRC
    1 = SC
    2 = benchmark
    3 = benchmark NetSMF
    """

    # --------------------------------------------------------------------------
    # Blog catalog micro NetMF
    plt.subplot(2, 3, 1)
    x = f("10.00	20.00	30.00	40.00	50.00	60.00	70.00	80.00	90.00")
    y = [[] for i in range(n)]
    y[0] = f("37.03	39.87	41.29	42.35	42.77	43.16	43.77	44.19	44.43")
    y[1] = f("37.31	40.12	41.49	42.3	43.06	43.5	43.87	44.51	44.76")
    y[2] = f("38.31	40.91	42.29	42.98	43.78	44.15	44.2	44.4	44.81")
    y[3] = f("34.95	37.95	39.37	40.2	41.14	41.61	41.91	42.26	42.35")
    for i in range(n):
        plt.plot(x, y[i], label=labels[i], marker=markers[i])
    plt.grid()
    plt.title('BlogCatalog')
    plt.ylabel('Micro-F1 (%)', fontsize='large')

    # Blog catalog macro NetMF
    plt.subplot(2, 3, 4)
    y = [[] for i in range(n)]
    y[0] = f("19.53	23.07	25.12	26.45	27.1	27.53	27.92	28.02	27.91")
    y[1] = f("19.78	23.19	25.2	26.44	27.56	28.21	28.41	28.49	28.06")
    y[2] = f("20.29	23.56	25.72	27.07	28.13	28.53	28.54	28.22	27.56")
    y[3] = f("15.72	19.24	20.92	22.15	23.26	23.59	23.89	24.09	23.72")
    for i in range(n):
        plt.plot(x, y[i], label=labels[i], marker=markers[i])
    plt.grid()
    plt.ylabel('Macro-F1 (%)', fontsize='large')

    # --------------------------------------------------------------------------
    # Flickr micro
    plt.subplot(2, 3, 2)
    x = f("1.00	2.00	3.00	4.00	5.00	6.00	7.00	8.00	9.00	10.00")
    y = [[] for i in range(n)]
    y[0] = f("29.31	32.26	33.65	34.56	35.13	35.64	36.01	36.33	36.57	36.82")
    y[1] = f("30.03	32.48	33.86	34.71	35.32	35.82	36.18	36.47	36.78	36.97")
    y[2] = f("30.28	32.34	33.52	34.3	34.9	35.35	35.63	35.94	36.22	36.4")
    y[3] = f("28.57	30.42	31.59	32.43	33.02	33.47	33.83	34.18	34.5	34.74")
    for i in range(n):
        plt.plot(x, y[i], label=labels[i], marker=markers[i])
    plt.grid() 
    plt.title('Flickr')

    # Flickr macro
    plt.subplot(2, 3, 5)
    y = [[] for i in range(n)]
    y[0] = f("9.7	12.87	15.08	16.51	17.45	18.47	19.11	19.76	20.35	20.91")
    y[1] = f("10.41	13.42	15.37	16.77	17.7	18.68	19.35	20.03	20.59	20.96")
    y[2] = f("9.71	12.4	14.2	15.47	16.39	17.25	17.79	18.46	19.02	19.35")
    y[3] = f("6.49	8.06	9.27	10.3	11.05	11.78	12.37	13	13.53	13.97")
    for i in range(n):
        plt.plot(x, y[i], label=labels[i], marker=markers[i])
    plt.grid()

    # --------------------------------------------------------------------------
    # YouTube micro
    plt.subplot(2, 3, 3)
    x = f("1.00	2.00	3.00	4.00	5.00	6.00	7.00	8.00	9.00	10.00")
    y = [[] for i in range(n)]
    y[0] = f("40.57	42.30	43.34	43.93	44.22	44.60	44.84	45.02	45.17	45.33")
    y[1] = f("40.35	42.19	42.93	43.39	43.66	43.95	44.17	44.35	44.55	44.68")
    y[2] = f("32.56	33.85	34.34	34.75	35.08	35.26	35.45	35.60	35.77	35.83")
    y[3] = f("36.14	39.07	40.33	41.15	41.55	41.99	42.38	42.62	42.88	43.1")
    # Old Youtube NetSMF
    #y[3] = f("34.65	36.94	37.95	38.72	39.16	39.65	40.01	40.24	40.48	40.69")
    for i in range(n):
        plt.plot(x, y[i], label=labels[i], marker=markers[i])
    plt.grid() 
    plt.title('YouTube')

    # YouTube Line macro
    plt.subplot(2, 3, 6)
    y = [[] for i in range(n)]
    y[0] = f("30.59	33.39	34.79	35.47	35.97	36.36	36.68	36.94	37.19	37.43")
    y[1] = f("30.17	32.92	34.14	34.75	35.05	35.46	35.73	35.99	36.25	36.38")
    y[2] = f("19.75	21.97	22.93	23.44	23.92	24.19	24.45	24.64	24.92	25.05")
    y[3] = f("22.93	27.04	28.84	29.94	30.61	31.29	31.89	32.28	32.69	33.01")
    for i in range(n):
        plt.plot(x, y[i], label=labels[i], marker=markers[i])
    plt.grid()
    plt.legend(labels=algorithms, loc='lower center',
               bbox_to_anchor=(-0.70, -0.3), fancybox=False, shadow=False, ncol=n)

    plt.savefig("f1-netmf-embeddings.png", transparent=True, bbox_inches='tight', dpi=256)
    plt.show()

main()
