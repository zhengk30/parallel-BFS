def to_csv(read_from: str) -> str:
    write_to = 'finalgraph.txt'
    src = open(read_from, 'r')
    dest = open(write_to, 'w')

    for line in src:
        data = line.split(',')
        dest.write(data[0] + ' ' + data[1] + '\n')

    return write_to


if __name__ == '__main__':
    file = 'C:\\Users\\86176\\OneDrive\\Desktop\\graph.txt'
    write_to = to_csv(file)
    
