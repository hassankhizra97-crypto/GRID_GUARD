path = r'c:\Users\HP\OneDrive\Desktop\dsa-project\frontend\src\components\KarachiMap.tsx'
with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

# Remove JSX inline comments from closing div lines that are outside the JSX tree
replacements = [
    (
        '      </div>{/* closes map canvas (flex-1 w-full h-full relative dark-tiles) */}',
        '      </div>'
    ),
    (
        '      </div>{/* closes inner column (flex-1 flex flex-col) */}',
        '      </div>'
    ),
    (
        '    </div>{/* closes outer flex wrapper */}',
        '    </div>'
    ),
]

for old, new in replacements:
    if old in content:
        content = content.replace(old, new)
        print(f'Replaced: {old[:60]}...')
    else:
        print(f'NOT FOUND: {old[:60]}...')

with open(path, 'w', encoding='utf-8') as f:
    f.write(content)

print('Done')
