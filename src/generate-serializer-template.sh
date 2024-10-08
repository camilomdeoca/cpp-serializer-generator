#!/usr/bin/env sh

template_path=$1
out_filepath=$2

if [ ! -f $template_path ]
then
    exit 1
fi

> $out_filepath

echo "#include \"SerializerTemplate.hpp\"" >> $out_filepath
echo "" >> $out_filepath

echo 'const char *const SERIALIZER_HEADER_TEMPLATE =' >> $out_filepath
cat $template_path | while IFS=""; read -r line
do
    escaped_line=$(echo $line | sed 's/\\/\\\\\\\\/g' | sed 's/"/\\"/g')
    echo -e '"'$escaped_line'\\n"' >> $out_filepath
done

echo ";" >> $out_filepath


