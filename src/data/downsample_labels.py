def main():
    label_filename = 'facebook.group.txt'
    labeled_subset_filename = 'facebook_subset_1000.txt'
    new_label_subset_filename = 'facebook_1000.group.txt'

    # Read labels
    labels = {}
    with open(label_filename) as label_file:
        line = label_file.readline().strip()
        num_nodes, num_label_types, num_labels = map(int, line.split())
        for line in label_file.readlines():
            node, label = map(int, line.strip().split())
            label = 0       # want all the same labels
            if node not in labels:
                labels[node] = []
            labels[node] = [0]
            #labels[node].append(label)
    print(len(labels))

    # Read subset of labels
    subset_of_nodes = set()
    with open(labeled_subset_filename) as subset_file:
        line = subset_file.readline().strip()
        subset_size, node_size = map(int, line.split())
        assert(node_size == num_nodes)
        for line in subset_file.readlines():
            node = int(line.strip())
            subset_of_nodes.add(node)
    print(len(subset_of_nodes))

    # Write new labels file
    new_labels_types = set()
    new_label_mapper = {}
    num_new_labels = 0
    num_label_entries = 0
    for node in subset_of_nodes:
        for label_type in labels[node]:
            new_labels_types.add(label_type)
            num_label_entries += 1
            if label_type not in new_label_mapper:
                new_label_mapper[label_type] = num_new_labels
                num_new_labels += 1

    with open(new_label_subset_filename, 'w') as outfile:
        outfile.write(' '.join([str(num_nodes), str(len(new_labels_types)), str(num_label_entries)]) + '\n')
        for node in sorted(subset_of_nodes):
            for label in sorted(labels[node]):
                outfile.write(' '.join([str(node), str(new_label_mapper[label])]) + '\n')

main()
