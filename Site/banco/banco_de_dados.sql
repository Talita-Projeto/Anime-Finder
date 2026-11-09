create extension if not exists unaccent with schema extensions;


create table if not exists public.generos (
  id    smallint generated always as identity primary key,
  nome  text not null,
  constraint generos_nome_nao_vazio check (length(btrim(nome)) > 0)
);
create unique index if not exists generos_nome_unico
  on public.generos (lower(nome));

create table if not exists public.animes (
  id             bigint   generated always as identity primary key,
  titulo         text     not null,
  genero_id      smallint not null
                 references public.generos (id)
                 on update cascade on delete restrict,
  classificacao  smallint not null,   
  episodios      integer  not null,
  sinopse        text,                
  capa           text,                
  criado_em      timestamptz not null default now(),
  constraint animes_titulo_nao_vazio     check (length(btrim(titulo)) > 0),
  constraint animes_classificacao_valida check (classificacao in (0, 10, 12, 14, 16, 18)),
  constraint animes_episodios_positivo   check (episodios >= 1)
);

alter table public.animes add column if not exists sinopse text;
alter table public.animes add column if not exists capa    text;

create unique index if not exists animes_titulo_unico
  on public.animes (lower(titulo));
create index if not exists animes_genero_classificacao_idx
  on public.animes (genero_id, classificacao);


alter table public.generos enable row level security;
alter table public.animes  enable row level security;

revoke insert, update, delete, truncate on public.generos from anon, authenticated;
revoke insert, update, delete, truncate on public.animes  from anon, authenticated;


grant select on public.generos, public.animes to anon, authenticated;

drop policy if exists generos_leitura_publica on public.generos;
create policy generos_leitura_publica on public.generos
  for select to anon, authenticated using (true);

drop policy if exists animes_leitura_publica on public.animes;
create policy animes_leitura_publica on public.animes
  for select to anon, authenticated using (true);


create or replace view public.v_animes
with (security_invoker = true) as
select a.id, a.titulo, g.nome as genero, a.classificacao,
       a.episodios, a.sinopse, a.capa
from public.animes a
join public.generos g on g.id = a.genero_id;

create or replace view public.v_generos
with (security_invoker = true) as
select g.id, g.nome, count(a.id)::integer as total_animes
from public.generos g
left join public.animes a on a.genero_id = g.id
group by g.id, g.nome
order by g.nome;


create or replace function public.recomendar_animes(p_genero text, p_idade integer)
returns setof public.v_animes
language plpgsql
stable
security invoker
set search_path = ''
as $$
begin
  if p_idade is null or p_idade < 1 or p_idade > 120 then
    raise exception 'Idade invalida: informe um numero inteiro entre 1 e 120.'
      using errcode = '22023';
  end if;

  return query
    select v.*
    from public.v_animes v
    where extensions.unaccent(lower(v.genero))
          = extensions.unaccent(lower(btrim(p_genero)))
      and v.classificacao <= p_idade
    order by v.titulo;
end;
$$;


create or replace function public.buscar_animes(p_termo text)
returns setof public.v_animes
language plpgsql
stable
security invoker
set search_path = ''
as $$
begin
  if p_termo is null or btrim(p_termo) = '' then
    raise exception 'A busca nao pode ficar vazia.' using errcode = '22023';
  end if;

  return query
    select v.*
    from public.v_animes v
    where strpos(extensions.unaccent(lower(v.titulo)),
                 extensions.unaccent(lower(btrim(p_termo)))) > 0
    order by v.titulo;
end;
$$;

grant select on public.v_animes, public.v_generos to anon, authenticated;
grant execute on function public.recomendar_animes(text, integer) to anon, authenticated;
grant execute on function public.buscar_animes(text)              to anon, authenticated;


insert into public.generos (nome) values
  ('Aventura'), ('Suspense'), ('Comédia'), ('Romance'),
  ('Ação'), ('Esporte'), ('Fantasia'), ('Drama')
on conflict do nothing;

insert into public.animes (titulo, genero_id, classificacao, episodios)
select v.titulo, g.id, v.classificacao, v.episodios
from (values
  ('Naruto',                           'Aventura', 12,  220),
  ('One Piece',                        'Aventura', 12, 1100),
  ('Death Note',                       'Suspense', 16,   37),
  ('Spy x Family',                     'Comédia',  12,   37),
  ('Your Name',                        'Romance',  10,    1),
  ('Attack on Titan',                  'Ação',     16,   87),
  ('Demon Slayer',                     'Ação',     16,   63),
  ('Haikyuu',                          'Esporte',  10,   85),
  ('Frieren',                          'Fantasia', 12,   28),
  ('K-On',                             'Comédia',  10,   39),
  ('Fullmetal Alchemist: Brotherhood', 'Aventura', 14,   64),
  ('Violet Evergarden',                'Drama',    12,   13),
  ('Mob Psycho 100',                   'Ação',     12,   37),
  ('Dr. Stone',                        'Aventura', 12,   57),
  ('A Silent Voice',                   'Drama',    12,    1)
) as v (titulo, genero, classificacao, episodios)
join public.generos g on lower(g.nome) = lower(v.genero)
on conflict do nothing;

update public.animes a
set capa = 'https://cmvyjchtzocaxqqceerp.supabase.co/storage/v1/object/public/capas/' || v.arquivo
from (values
  ('Naruto',                           'Naruto-Classico-Capa.webp'),
  ('One Piece',                        'one-piece.jpeg'),
  ('Death Note',                       'death-note.jpg'),
  ('Spy x Family',                     'spy-x-family.jpg'),
  ('Your Name',                        'your-name.jpg'),
  ('Attack on Titan',                  'attack-on-titan.jpg'),
  ('Demon Slayer',                     'demon-slayer.webp'),
  ('Haikyuu',                          'haikyuu.webp'),
  ('Frieren',                          'frieren.webp'),
  ('K-On',                             'k-on.webp'),
  ('Fullmetal Alchemist: Brotherhood', 'fullmetal-alchemist-brotherhood.webp'),
  ('Violet Evergarden',                'violet-evergarden.webp'),
  ('Mob Psycho 100',                   'mob-psycho-100.webp'),
  ('A Silent Voice',                   'a-silent-voice.jpg'),
  ('Steins;Gate',                      'steins;gate.webp'),
  ('The Quintessential Quintuplets',   'the-quintessential-quintuplets.jpg'),
  ('Vivy: Fluorite Eye’s Song',        'vivy-fluorite-eyes-song.webp')
) as v (titulo, arquivo)
where lower(a.titulo) = lower(v.titulo);


notify pgrst, 'reload schema';

select (select count(*) from public.v_animes)  as animes,
       (select count(*) from public.v_generos) as generos;
